
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "trackedobject.hpp"

using tcp = boost::asio::ip::tcp;
namespace websocket = boost::beast::websocket;

//  Only one instance of the websocket server can be run at a time, due to the
//  global variable referencing the trackedobject pointer. Futhermore, only a
//  single TrackedObject can be used at a time.

// TODO: Refactor and remove this global variable
TrackedObject *SERVER_TRACKEDOBJECT_PTR;

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
            if(SERVER_TRACKEDOBJECT_PTR != nullptr){
                if(req == "head"){
                    ret = "blah";
                }
                else if(req == "all"){
                    ret = SERVER_TRACKEDOBJECT_PTR->get_all_past_pos_json();
                }

                else if(req == "predictland"){

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

int main(int argc, char* argv[]) {
    // Check command line arguments.
    if (argc != 4)
    {
        std::cerr <<
            "Usage: websocketserver <address> <port> <threads>\n" <<
            "Example:\n" <<
            "    websocketserver 0.0.0.0 8080 1\n";
        return EXIT_FAILURE;
    }
    init_server(argv[1], std::atoi(argv[2]), std::atoi(argv[3]), nullptr);

    return EXIT_SUCCESS;
}