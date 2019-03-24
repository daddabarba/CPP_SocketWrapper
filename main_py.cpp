//
// Created by daddabarba on 3/22/19.
//
#include <string>
#include <iostream>

#include "cppsockets/SocketClient.h"

int main(int argc, char **argv){

    std::cout << "port: " << argv[1] << std::endl;

    std::string msg;
    auto client = skt::SocketClient(argv[1]);
    client.start_connection();

    std::cout << "connected" << std::endl;
    client >> msg;
    std::cout << "Message: " << msg << std::endl;
    client << msg << "_2";

    return 0;
}