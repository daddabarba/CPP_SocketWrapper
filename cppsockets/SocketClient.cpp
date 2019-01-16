//
// Created by daddabarba on 1/16/19.
//

#include "SocketClient.h"

#include <stdexcept>

#include <cstring>

#include <netdb.h>

// Constructor


skt::SocketClient::SocketClient(uint16_t port, const std::string host_name, int domain, size_t buffer_size) :
    Socket(port, domain, buffer_size)
{
    this->socket_fd = socket(this->domain, SOCK_STREAM, 0);

    if(this->socket_fd < 0)
        throw std::runtime_error("ERROR opening socket");

    struct hostent *server = gethostbyname(host_name.c_str());

    if(server == nullptr)
        throw std::runtime_error("ERROR, no such host");

    memset((char *)& this->server_addr, 0, sizeof(this->server_addr));

    this->server_addr.sin_family = (sa_family_t)domain;
    bcopy((char *)server->h_addr,
          (char *)& this->server_addr.sin_addr.s_addr,
          (size_t)server->h_length);
    this->server_addr.sin_port = port;
}

// Socket handler

skt::Socket& skt::SocketClient::start_connection() {

    if (connect(this->socket_fd,(struct sockaddr *)& this->server_addr, sizeof(this->server_addr)) < 0)
        throw std::runtime_error("ERROR connecting");

    return *this;
}
