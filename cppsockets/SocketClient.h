//
// Created by daddabarba on 1/16/19.
//

#ifndef CPPSOCKETS_SOCKETCLIENT_H
#define CPPSOCKETS_SOCKETCLIENT_H

#include "Socket.h"

#include <string>

namespace skt {

    class SocketClient : public Socket {

    public:

        //Constructor
        explicit SocketClient(uint16_t port, const std::string ip, int domain = AF_INET, size_t buffer_size = skt::BUFFER_SIZE);

        Socket& start_connection() override; //connect to other end of socket

    };
}


#endif //CPPSOCKETS_SOCKETCLIENT_H
