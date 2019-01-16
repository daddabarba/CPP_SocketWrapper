//
// Created by daddabarba on 1/16/19.
//

#include "SocketClient.h"

#include <exception>
#include <stdexcept>

#include <cstring>

#include <netdb.h>

// Constructor

template <typename T> skt::SocketClient<T>::SocketClient(uint16_t port, char* host_name, int domain, size_t buffer_size) :
    Socket<T>(port, domain, buffer_size)
{
    skt::Socket<T>::socket_fd = socket(skt::Socket<T>::domain, SOCK_STREAM, 0);

    if(skt::Socket<T>::socket_fd < 0)
        throw std::runtime_error("ERROR opening socket");

    struct hostent *server = gethostbyname(host_name);

    if(server == nullptr)
        throw std::runtime_error("ERROR, no such host");

    memset((char *)& skt::Socket<T>::server_addr, 0, sizeof(this->server_addr));

    skt::Socket<T>::server_addr.sin_family = (sa_family_t)domain;
    bcopy((char *)server->h_addr,
          (char *)& skt::Socket<T>::server_addr.sin_addr.s_addr,
          (size_t)server->h_length);
    skt::Socket<T>::server_addr.sin_port = port;
}

// Socket handler

template<typename T> skt::Socket<T>& skt::SocketClient<T>::start_connection() {

    if (connect(skt::Socket<T>::socket_fd,(struct sockaddr *)& skt::Socket<T>::server_addr, sizeof(skt::Socket<T>::server_addr)) < 0)
        throw std::runtime_error("ERROR connecting");

    return *this;
}
