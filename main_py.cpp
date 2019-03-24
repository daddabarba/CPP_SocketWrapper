//
// Created by daddabarba on 3/22/19.
//
#include <string>
#include <iostream>

#include "cppsockets/SocketClient.h"

int main(int argc, char **argv){

    std::cout << "port: " << argv[1] << std::endl;

    int val;
    auto client = skt::SocketClient(argv[1]);
    client.start_connection();

    std::cout << "connected" << std::endl;
    client >> &val;
    std::cout << "Message: " << val << std::endl;
    client.reset_buffer();
    client >> &val;
    std::cout << "Message: " << val << std::endl;

    return 0;
}