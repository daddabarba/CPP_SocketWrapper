//
// Created by daddabarba on 1/15/19.
//

#include "SocketServer.h"

#include <exception>
#include <stdexcept>

#include <cstring>
#include <unistd.h>

template <typename T> skt::SocketServer<T>::SocketServer(uint16_t port, int domain, size_t buffer_size) :
        Socket<T>(port, domain, buffer_size),
        server_fd(socket(domain, SOCK_STREAM, 0)) // Opening socket
{

    if(this->server_fd < 0)
        throw std::runtime_error("ERROR opening socket");

    // Setting socket address
    memset((char *) &skt::Socket<T>::server_addr, 0, sizeof(this->server_addr));

    skt::Socket<T>::server_addr.sin_family = (sa_family_t)domain;
    skt::Socket<T>::server_addr.sin_addr.s_addr = INADDR_ANY; //run on localhost
    skt::Socket<T>::server_addr.sin_port = htons(port);

    // Bind socket to address
    if (bind(server_fd, (struct sockaddr *) &skt::Socket<T>::server_addr, sizeof(skt::Socket<T>::server_addr)) < 0)
        throw std::runtime_error("ERROR on binding");

    // Start socket
    listen(server_fd, skt::BACKLOG); // listen at port and address
}

// Socket handler

template<typename T> skt::Socket<T>& skt::SocketServer<T>::start_connection() {
    socklen_t clilen = sizeof(client_addr);
    skt::Socket<T>::socket_fd = accept(server_fd, (struct sockaddr *) &client_addr, &clilen); // wait for client

    if(skt::Socket<T>::socket_fd < 0)
        throw std::runtime_error("ERROR on accept");

    return *this;
}

template<typename T> skt::Socket<T>& skt::SocketServer<T>::close_socket() {
    skt::Socket<T>::close_socket();
    close(server_fd);

    return *this;
}