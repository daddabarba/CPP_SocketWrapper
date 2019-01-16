//
// Created by daddabarba on 1/16/19.
//

#ifndef CPPSOCKETS_SOCKETCLIENT_H
#define CPPSOCKETS_SOCKETCLIENT_H

#include "Socket.h"

namespace skt {
    template<typename T>
    class SocketClient : public Socket<T> {

    public:

        //Constructor
        explicit SocketClient(uint16_t port, char* host_name, int domain = AF_INET, size_t buffer_size = skt::BUFFER_SIZE);

        Socket<T> &start_connection() override; //connect to other end of socket

    };
}


#endif //CPPSOCKETS_SOCKETCLIENT_H
