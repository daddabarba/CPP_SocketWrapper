//
// Created by daddabarba on 1/12/19.
//

#ifndef CPPSOCKETS_SOCKETSERVER_H
#define CPPSOCKETS_SOCKETSERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <cstring>

#define BACKLOG 5
#define BUFFER_SIZE 256

const char* STOP_MESSAGE = nullptr;
const int READ_ALL = -1;

const bool (*DEF_FREE_BUFF)(char*) = [=](char*)->bool{ return true;};
char* (*DEFAULT_INIT_COMMUNICATIONS)() = [=]()->char*{return nullptr};

class SocketServer {

public:

    typename struct Mem {
        typedef struct BufferData {

            char *&buffer;
            size_t &buffer_size;
            int &buffer_max;

            BufferData(char *&buffer, size_t &buffer_size, int &buffer_max);

        } BufferData;

        BufferData data;

    }Mem;

    // Constructors
    explicit SocketServer(uint16_t port, int domain=AF_INET, size_t buffer_size=BUFFER_SIZE);

    // Destructor
    ~SocketServer();

    // Methods

    void start();

    void init(char* (*init_communication)() = DEFAULT_INIT_COMMUNICATIONS);
    template<typename T> void loop(char* (*handle)(char*, T&), bool (*stop)(char*, T&), T& mem, int max=-1, int depth=0);

    SocketServer& operator<<(const char* message);
    SocketServer& operator<< (std::string const& message);

    SocketServer& operator>>(std::string& buffer);
    SocketServer& operator>>(int max);

    char* get_buffer();
    void reset_buffer();

    //Delete copy from constant reference
    SocketServer(const SocketServer& mc) 				= delete;
    SocketServer operator= (const SocketServer& mc) 	= delete;

private:
    size_t buffer_size;
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;

    char *buffer;
    int buffer_max;

    void validate_mem(Mem mem) const {};
    template<typename T> void loop_aux(char* (*handle)(char*, T&), bool (*stop)(char*, T&), T& mem, int max, int depth);

    char* get();
    char* get(int max);
    void send(const char* message);

};


#endif //CPPSOCKETS_SOCKETSERVER_H
