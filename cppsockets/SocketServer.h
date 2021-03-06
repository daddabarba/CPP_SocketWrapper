//
// Created by daddabarba on 1/15/19.
//

#ifndef CPPSOCKETS_SOCKETSERVER_H
#define CPPSOCKETS_SOCKETSERVER_H

#include <sys/socket.h>
#include <netinet/in.h>

#include "Socket.h"

namespace skt {

    class SocketServer : public Socket {

    public:

        // Constructors
        explicit SocketServer(uint16_t port, size_t buffer_size = skt::BUFFER_SIZE);
        explicit SocketServer(const std::string file_name, size_t buffer_size = skt::BUFFER_SIZE);
        ~SocketServer();

        Socket& start_connection() override; //connect to other end of socket

    protected:

        SocketServer(int domain, size_t buffer_size);

        int server_fd;
        struct sockaddr_in client_addr;

        void bind_sock();

    };
}

#endif //CPPSOCKETS_SOCKETSERVER_H
