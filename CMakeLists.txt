cmake_minimum_required(VERSION 3.19)
project(tcp_hello_world)

set(CMAKE_CXX_STANDARD 20)

add_executable(tcp_hello_server src/HelloServer.cpp)
add_executable(tcp_hello_client src/HelloClient.cpp)