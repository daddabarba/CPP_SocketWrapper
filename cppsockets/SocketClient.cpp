//
// Created by daddabarba on 1/16/19.
//

#include "SocketClient.h"

#include <iostream>

#include <cstring>
#include <arpa/inet.h>

// Constructor


skt::SocketClient::SocketClient(uint16_t port, const std::string ip, int domain, size_t buffer_size) :
    Socket(port, domain, buffer_size)
{
    this->socket_fd = socket(this->domain, SOCK_STREAM, 0);

    if(this->socket_fd < 0)
        throw std::runtime_error("ERROR opening socket");

    memset((char *)& this->server_addr, 0, sizeof(this->server_addr));

    this->server_addr.sin_family = (sa_family_t)domain;
    this->server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    this->server_addr.sin_port = htons((u_short)port);
}

// Socket handler

skt::Socket& skt::SocketClient::start_connection() {

    if (connect(this->socket_fd,(struct sockaddr *)& this->server_addr, sizeof(this->server_addr)) < 0)
        throw std::runtime_error("ERROR connecting");

    return *this;
}
