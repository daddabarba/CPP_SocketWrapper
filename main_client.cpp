#include <iostream>

#include "cppsockets/SocketClient.h"

class StoreSomething{
public:
    int x;

    StoreSomething() :
            x(0)
    {};
};

int main(int argc, char** argv) {

    StoreSomething *storeSomething = new StoreSomething();
    skt::Mem memory((void *)storeSomething);

    (*(new skt::SocketClient(7778, "10.211.55.3")))
            .set_handler((*(new skt::Handler()))
                                 .set_handler_function([&](char* buffer, skt::Mem& mem)->char*{

                                     for(int i=0; i<*mem.data.buffer_max; i++) {
                                         printf("%c->%c", buffer[i], (buffer[i] + 1) % 30);
                                         buffer[i] = (buffer[i] + 1) % 30;
                                     }
                                     printf("\n");

                                     printf("Previously red: %d", ((StoreSomething *)mem.obj)->x);
                                     ((StoreSomething *)mem.obj)->x = *mem.data.buffer_max;

                                     return buffer;

                                 })
                                 .set_stop_handler([&](char* buffer, skt::Mem& mem)->bool{

                                     mem.data.socket_interface->reset_buffer();
                                     return true;
                                 })
                                 .set_mem(memory))
            .start_handler("ca", 6);

    return 0;
}