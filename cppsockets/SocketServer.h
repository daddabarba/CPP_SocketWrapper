//
// Created by daddabarba on 1/15/19.
//

#ifndef CPPSOCKETS_SOCKETSERVER_H
#define CPPSOCKETS_SOCKETSERVER_H

#include <sys/socket.h>
#include <netinet/in.h>

#include "Socket.h"

namespace skt {

    template <typename T>
    class SocketServer : public Socket<T> {

    public:
        // Constructors
        explicit SocketServer(uint16_t port, int domain = AF_INET, size_t buffer_size = skt::BUFFER_SIZE);

        Socket<T> &start_connection() override;
        Socket<T> &close_socket() override ;

    protected:
        int server_fd;
        struct sockaddr_in client_addr;

    };
}

#endif //CPPSOCKETS_SOCKETSERVER_H
