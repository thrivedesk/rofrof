cmake_minimum_required(VERSION 3.17)
project(rof_rof)

set(CMAKE_CXX_STANDARD 20)

INCLUDE_DIRECTORIES(uWebSockets/src uWebSockets/uSockets/src)
add_executable(rof_rof main.cpp)
