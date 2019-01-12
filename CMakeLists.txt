cmake_minimum_required(VERSION 3.8)
project(CPPSockets)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp cppsockets/SocketServer.cpp cppsockets/SocketServer.h)
add_executable(CPPSockets ${SOURCE_FILES})