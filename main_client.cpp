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
            output("/home/daddabarba/Desktop/output_client.txt")
    {};

    ~StoreSomething(){
        this->output <<"end"<< std::endl;
        this->output.close();
    }
};

int main(int argc, char** argv) {


    auto obj = new StoreSomething();

    (*(new skt::SocketClient(7779, "127.0.0.1")))
            .set_handler((*(new skt::Handler()))
                                 .set_handler_function([&obj](char* buffer, skt::BufferData& mem)->char*{

                                     for(int i=0; i<*mem.buffer_max; i++) {
                                         obj->output<< (char)buffer[i] << "->"<< (char)((buffer[i] - 96) % 26 + 97)<<" ";
                                         buffer[i] = ((buffer[i] -96) % 26 + 97);
                                     }
                                     obj->output << "\t";

                                     obj->output<<"Previously red: " << obj->x << std::endl;

                                     std::this_thread::sleep_for(std::chrono::milliseconds(10));

                                     return buffer;

                                 })
                                 .set_stop_handler([&](char* buffer, skt::BufferData& mem)->bool{

                                     mem.socket_interface->reset_buffer();
                                     return false;
                                 }))
            .start_handler("ciaoFrancesca", 6);


    return 0;
}