//
// Created by daddabarba on 1/16/19.
//

#include "SocketClient.h"

#include <iostream>

#include <cstring>
#include <arpa/inet.h>

// Constructor


skt::SocketClient::SocketClient(int domain, size_t buffer_size) :
    Socket(domain, buffer_size)
{};

skt::SocketClient::SocketClient(uint16_t port, const std::string ip, size_t buffer_size) :
    SocketClient(AF_INET, buffer_size)
{
    memset((char*)&this->server_addr_in, 0, sizeof(this->server_addr_in));

    this->server_addr_in.sin_family = (sa_family_t)domain;
    this->server_addr_in.sin_addr.s_addr = inet_addr(ip.c_str());
    this->server_addr_in.sin_port = htons((u_short)port);
}

skt::SocketClient::SocketClient(const std::string file_name, size_t buffer_size) :
    SocketClient(AF_UNIX, buffer_size)
{
    memset((char*)&this->server_addr_un, 0, sizeof(this->server_addr_un));

    this->server_addr_un.sun_family = AF_UNIX;
    strcpy(this->server_addr_un.sun_PATH, file_name.c_str());
}

// Socket handler

auto skt::SocketClient::start_connection() -> skt::Socket& {

    if (
        (
            this->domain == AF_INET
            && connect(
                    this->socket_fd,
                    (struct sockaddr*)&this->server_addr_in,
                    sizeof(this->server_addr_in)) < 0)
        || (
            this->domain == AF_UNIX
            && connect(
                    this->socket_fd,
                    (struct sockaddr*)&this->server_addr_un,
                    sizeof(this->server_addr_un)) < 0))
        throw std::runtime_error("ERROR connecting");

    return *this;
}
