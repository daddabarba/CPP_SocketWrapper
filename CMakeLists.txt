cmake_minimum_required(VERSION 3.8)
project(CPPSockets)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES ${SOURCES}
        cppsockets/Socket.cpp
        cppsockets/Socket.h
        cppsockets/SocketServer.cpp
        cppsockets/SocketServer.h
        cppsockets/SocketClient.cpp
        cppsockets/SocketClient.h
)

add_executable(MainServer main_server.cpp ${SOURCE_FILES})
add_executable(MainClient main_client.cpp ${SOURCE_FILES})
add_executable(MainPy main_py.cpp ${SOURCE_FILES})