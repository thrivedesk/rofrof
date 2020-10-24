cmake_minimum_required(VERSION 3.17)
project(rofrof)

set(CMAKE_CXX_STANDARD 20)

INCLUDE_DIRECTORIES(uWebSockets/src uWebSockets/uSockets/src)
add_executable(rofrof main.cpp)
