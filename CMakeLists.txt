cmake_minimum_required(VERSION 3.8)
project(CPPSockets)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main_server.cpp cppsockets/Socket.cpp cppsockets/Socket.h cppsockets/SocketServer.cpp cppsockets/SocketServer.h cppsockets/SocketClient.cpp cppsockets/SocketClient.h)
add_executable(MainServer ${SOURCE_FILES})

set(SOURCE_FILES main_client.cpp cppsockets/Socket.cpp cppsockets/Socket.h cppsockets/SocketServer.cpp cppsockets/SocketServer.h cppsockets/SocketClient.cpp cppsockets/SocketClient.h)
add_executable(MainClient ${SOURCE_FILES})