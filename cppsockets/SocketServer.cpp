//
// Created by daddabarba on 1/15/19.
//

#include "SocketServer.h"

#include <stdexcept>

#include <cstring>
#include <unistd.h>


#include <arpa/inet.h>

skt::SocketServer::SocketServer(uint16_t port, int domain, size_t buffer_size) :
        Socket(port, domain, buffer_size),
        server_fd(socket(domain, SOCK_STREAM, 0)) // Opening socket
{

    if(this->server_fd < 0)
        throw std::runtime_error("ERROR opening socket");

    // Setting socket address
    memset((char *) &(this->server_addr), 0, sizeof(skt::Socket::server_addr));

    this->server_addr.sin_family = (sa_family_t)domain;
    this->server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //run on localhost
    this->server_addr.sin_port = htons((u_short)port);

    // Bind socket to address
    if (bind(server_fd, (struct sockaddr *) &this->server_addr, sizeof(this->server_addr)) < 0)
        throw std::runtime_error("ERROR on binding");

    // Start socket
    listen(server_fd, skt::BACKLOG); // listen at port and address
}

// Socket handler

auto skt::SocketServer::start_connection() -> skt::Socket& {
    socklen_t clilent = sizeof(client_addr);
    this->socket_fd = accept(server_fd, (struct sockaddr *) &client_addr, &clilent); // wait for client

    if(this->socket_fd < 0)
        throw std::runtime_error("ERROR on accept");

    return *this;
}

// Destructor

skt::SocketServer::~SocketServer() {
    this->~Socket();
    close(server_fd);
}