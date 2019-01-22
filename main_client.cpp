#include "cppsockets/SocketClient.h"

#include <thread>
#include <chrono>

#include <fstream>

class StoreSomething{
public:
    int x;
    std::fstream output;

    StoreSomething() :
            x(0),
            output("./output_client.txt")
    {};

    ~StoreSomething(){
        this->output <<"end"<< std::endl;
        this->output.close();
    }
};

int main(int argc, char** argv) {

    StoreSomething *storeSomething = new StoreSomething();
    skt::Mem memory((void *)storeSomething);

    (*(new skt::SocketClient(7779, "127.0.0.1")))
            .set_handler((*(new skt::Handler()))
                                 .set_handler_function([&](char* buffer, skt::Mem& mem)->char*{

                                     StoreSomething *obj = ((StoreSomething *)mem.obj);

                                     for(int i=0; i<*mem.data.buffer_max; i++) {
                                         obj->output<< (char)buffer[i] << "->"<< (char)((buffer[i] - 96) % 26 + 97)<<" ";
                                         buffer[i] = ((buffer[i] -96) % 26 + 97);
                                     }
                                     obj->output << "\t";

                                     obj->output<<"Previously red: " << obj->x << std::endl;
                                     ((StoreSomething *)mem.obj)->x = *mem.data.buffer_max;

                                     std::this_thread::sleep_for(std::chrono::milliseconds(10));

                                     return buffer;

                                 })
                                 .set_stop_handler([&](char* buffer, skt::Mem& mem)->bool{

                                     mem.data.socket_interface->reset_buffer();
                                     return false;
                                 })
                                 .set_mem(memory))
            .start_handler("ciaoFrancesca", 6);


    return 0;
}