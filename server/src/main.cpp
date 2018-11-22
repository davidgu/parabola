#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <unistd.h>
#include <fcntl.h>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "opencv2/opencv.hpp"

#include "detector.hpp"
#include "simclock.hpp"
#include "trackedobject.hpp"


//
//  Global Variables
//
TrackedObject tobject;
TrackedObject *SERVER_TRACKEDOBJECT_PTR;
SimClock simClock;
bool DEBUG = false;

//  Notes and Errata
//  * sometimes killing the client while connected to the websocket server 
//    will cause the server to die as well.


// Server code
using tcp = boost::asio::ip::tcp;
namespace websocket = boost::beast::websocket;

//  Only one instance of the websocket server can be run at a time, due to the
//  global variable referencing the trackedobject pointer. Futhermore, only a
//  single TrackedObject can be used at a time.


// Report a failure
void
fail(boost::system::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

class session : public std::enable_shared_from_this<session> {
    websocket::stream<tcp::socket> ws_;
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
    boost::beast::multi_buffer buffer_;

    public:
        // Take ownership of the socket
        explicit session(tcp::socket socket) : ws_(std::move(socket)) , strand_(ws_.get_executor()){

        }

        // Start the asynchronous operation
        void run() {
            // Accept the websocket handshake
            ws_.async_accept(
                boost::asio::bind_executor(
                    strand_,
                    std::bind(
                        &session::on_accept,
                        shared_from_this(),
                        std::placeholders::_1)));
        }

        void on_accept(boost::system::error_code ec) {
            if(ec)
                return fail(ec, "accept");

            // Read a message
            do_read();
        }

        void do_read() {
            // Read a message into our buffer
            ws_.async_read(
                buffer_,
                boost::asio::bind_executor(
                    strand_,
                    std::bind(
                        &session::on_read,
                        shared_from_this(),
                        std::placeholders::_1,
                        std::placeholders::_2)));
        }

        void on_read(boost::system::error_code ec, std::size_t bytes_transferred) {
            boost::ignore_unused(bytes_transferred);

            // This indicates that the session was closed
            if(ec == websocket::error::closed)
                return;

            if(ec){
                fail(ec, "read");
            }

            // Echo the message
            ws_.text(ws_.got_text());

            std::string req = boost::beast::buffers_to_string(buffer_.data());
            std::cout << "Received: " <<  req << std::endl;

            std::string ret;

            if(DEBUG){
                if(req == "curpos"){
                    ret = "curpos:Returning curpos!";
                }
                else if(req == "all"){
                    ret = "all:Returning all!";
                }

                else if(req == "predictland"){
                    ret = "predictland:Returning landing location!";
                }
                else if(req == "predictpath"){
                    ret = "predictpath:Returning predicted path!";
                }
                else{
                    ret = "error:Invalid Request!";
                }
            }
            else if(SERVER_TRACKEDOBJECT_PTR != nullptr){
                if(req == "curpos"){
                    int ppos_len = (int)SERVER_TRACKEDOBJECT_PTR->get_all_past_tvpair().size();
                    if(ppos_len != 0){
                        ret = "curpos#";
                        ret += SERVER_TRACKEDOBJECT_PTR->get_tvpair_json(ppos_len - 1);
                    }
                    else{
                        ret = "curpos#error";
                    }
                }
                else if(req == "all"){
                    ret = "all:"+SERVER_TRACKEDOBJECT_PTR->get_all_past_tvpair_json();
                }

                else if(req == "predictland"){
                    Vector3 pred_pos = SERVER_TRACKEDOBJECT_PTR->predict_landing_pos();
                    if(pred_pos.x != std::numeric_limits<double>::max()){
                        ret = "predictland#"+pred_pos.to_json();
                    }
                    else{
                        // Return invalid statement, which will cause client to do nothing
                        ret = "predictland#error";
                    }
                }

                else if(req == "predictpath"){

                }
                else{
                    ret = "Invalid Request!";
                }
            }
            else{
                ret = "Error: TrackedObject reference unset!";
            }

            // Return data
            auto buf = boost::asio::buffer(&ret[0], ret.size());
            ws_.async_write(
                buf,
                boost::asio::bind_executor(
                    strand_,
                    std::bind(
                        &session::on_write,
                        shared_from_this(),
                        std::placeholders::_1,
                        std::placeholders::_2)));
        }

        void on_write(boost::system::error_code ec, std::size_t bytes_transferred) {
            boost::ignore_unused(bytes_transferred);

            if(ec)
                return fail(ec, "write");

            // Clear the buffer
            buffer_.consume(buffer_.size());

            // Do another read
            do_read();
        }
};

// Accepts incoming connections and launches the sessions
class listener : public std::enable_shared_from_this<listener> {
    tcp::acceptor acceptor_;
    tcp::socket socket_;

    public:
        listener(boost::asio::io_context& ioc, tcp::endpoint endpoint) : acceptor_(ioc), socket_(ioc) {
            boost::system::error_code ec;

            // Open the acceptor
            acceptor_.open(endpoint.protocol(), ec);
            if(ec)
            {
                fail(ec, "open");
                return;
            }

            // Allow address reuse
            acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
            if(ec)
            {
                fail(ec, "set_option");
                return;
            }

            // Bind to the server address
            acceptor_.bind(endpoint, ec);
            if(ec)
            {
                fail(ec, "bind");
                return;
            }

            // Start listening for connections
            acceptor_.listen(
                boost::asio::socket_base::max_listen_connections, ec);
            if(ec)
            {
                fail(ec, "listen");
                return;
            }
        }

        // Start accepting incoming connections
        void run() {
            if(! acceptor_.is_open()){
                return;
            }
            do_accept();
        }

        void do_accept() {
            acceptor_.async_accept(
                socket_,
                std::bind(
                    &listener::on_accept,
                    shared_from_this(),
                    std::placeholders::_1));
        }

        void on_accept(boost::system::error_code ec) {
            if(ec) {
                fail(ec, "accept");
            }
            else {
                // Create the session and run it
                auto sess = std::make_shared<session>(std::move(socket_));
                sess->run();
            }

            // Accept another connection
            do_accept();
        }
};

// Initialize websocket server
void init_server(std::string addr, int por, int threa, TrackedObject *tobj){
    auto const address = boost::asio::ip::make_address(addr);
    auto const port = static_cast<unsigned short>(por);
    auto const threads = std::max<int>(1, threa);

    // The io_context is required for all I/O
    boost::asio::io_context ioc{threads};

    // Create and launch a listening port
    std::make_shared<listener>(ioc, tcp::endpoint{address, port})->run();

    // Set trackedobject pointer
    ::SERVER_TRACKEDOBJECT_PTR = tobj;

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for(auto i = threads - 1; i > 0; --i)
        v.emplace_back(
        [&ioc]
        {
            ioc.run();
        });
    ioc.run();
}

int get_arduino(std::string arduino_device){
    int fd = open(arduino_device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1){
        exit(-1); // If the device is not open, return -1
    }
    return fd;
}

int main(int argc, char *argv[]){
    // arg 1 -> day/night (1/0)
    // arg 2 -> server ip
    // arg 3 -> server port

    //
    //  Initialize detector
    //
    Detector detector = Detector(
        (bool)std::stoi(argv[1]),
        (Mat1d(3, 3)<< 4.4740797894345377e+02, 0.0, 320.0, 0.0, 4.4740797894345377e+02, 213.0, 0.0, 0.0, 1.0),
        (Mat1d(1, 5)<< 1.7657493713843387e-01, -1.6646113914955049e-01, 0.0, 0.0, 2.6525645359464572e-01)
    );

    detector.configure_cameras();
    // No saved data exists
    if(detector.load_configuration("cpm1.xml", "cpm2.xml", "vo.xml")==0){
        std::cout << "No configuration files found!" << std::endl;
        detector.calibrate(0, 1);
        detector.calibrate_vorigin(0, 1);
        detector.save_configuration("cpm1.xml", "cpm2.xml", "vo.xml");
    }
    // Virtual origin data does not exist 
    else if(detector.load_configuration("cpm1.xml", "cpm2.xml", "vo.xml") == 1){
        std::cout << "Virtual origin configuration not found!" << std::endl;
        detector.calibrate_vorigin(0, 1);
        detector.save_configuration("cpm1.xml", "cpm2.xml", "vo.xml");
    }
    // All data exists
    else if(detector.load_configuration("cpm1.xml", "cpm2.xml", "vo.xml") == 2){
        std::cout << "All configuration found!" << std::endl;
        // Do nothing
    }
    else{
        // Fail if an invalid load configuration value is returned
        assert(false);
    }

    // Initialize arduino
    int arduino =  get_arduino(argv[4]);

    //
    //  Initialize server and start tracking
    //
    std::thread t1(init_server,argv[2], std::stoi(argv[3]), 1, &tobject);
    simClock = SimClock();
    while(true){
        cv::Point3f curPoint = detector.get_normalized_position();
        // If the detected point is valid, do stuff with it
        if(curPoint.x != std::numeric_limits<float>::max()){
            Vector3 curPointv = Vector3::point3f_to_vector3(curPoint);
            tobject.add_pos(simClock.get_abstime(), curPointv);
            std::cout << "Added (" << simClock.get_abstime() << "," << curPointv << ") to tobject" <<std::endl;

            // Write to Arduino
            int cmd = 1;
            write(arduino, &cmd, sizeof(cmd) -1 );
        }
    }
}