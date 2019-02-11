# Parabola
A computer vision object trajectory prediction engine, with an augmented reality client. Named after the path traced by an object experiencing free-fall.

## Description
### Server
* Written in C++ with Boost and OpenCV
* Reads image data from cameras, processes image data, and identifies position of tracked object
* Predicts future position and landing location of tracked object
* Provides websockets based API to retrieve tracked object data and object position predictions
### Client
* Written in C# with Unity and ARKit
* Connects to websockets server and retrieves tracked object data
* Displays tracked object path trails and future predicted positions in augmented reality

## Dependencies
* Boost
* OpenCV
* Unity 2018.3


Note: To build the client, a macOS machine is required.

## Building
To build the server, run the following commands from the root of the repository:
```
cd server/src/
g++-8 -std=c++11 -march=native $(pkg-config --cflags --libs opencv) -lboost_system -lboost_thread-mt -lboost_chrono-mt -lssl -lcrypto -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib *.cpp -o main.o
```
To build the client, load the Unity project and build through Unity and XCode.
