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

class SocketServer {

public:
    // Constructors
    explicit SocketServer(uint16_t port, size_t buffer_size=BUFFER_SIZE, int domain=AF_INET);

    // Destructor
    ~SocketServer();

    // Methods

    void init(char* (&init_communication)());
    void loop(char* (&handle)(char*), bool (&stop)(char*), bool (&free_buff)(char*) = *([=](char*)->bool{ return true;}), int max=-1, int depth=0);

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

    char* get();
    char* get(int max);
    void send(const char* message);

};


#endif //CPPSOCKETS_SOCKETSERVER_H
