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

static const char* STOP_MESSAGE = nullptr;
static const int READ_ALL = -1;

//static char* (*DEFAULT_INIT_COMMUNICATIONS)() = [&]()->char*{return nullptr;};

template<typename T>
class SocketServer {

public:

    typedef struct Mem {
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

    SocketServer& start_handler(T &mem, char *first_message = STOP_MESSAGE, int max = -1, int depth = 0);

    SocketServer& start_connection();
    SocketServer& close_socket();

    SocketServer& init_stream(char *first_message = STOP_MESSAGE);

    //Setters
    SocketServer& set_handler(char* (*handler_function)(char*, T&));

    //Operators
    SocketServer& operator<<(const char* message);
    SocketServer& operator<< (std::string const& message);

    SocketServer& operator>>(std::string& buffer);
    SocketServer& operator>>(int max);

    char* get_buffer();
    SocketServer& reset_buffer();

    //Delete copy from constant reference
    SocketServer(const SocketServer& mc) 				= delete;
    SocketServer operator= (const SocketServer& mc) 	= delete;

private:
    size_t buffer_size;
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;

    char *buffer;
    int buffer_max;

    char* (*handler)(char*, T&);
    bool (*stop_handler)(char *, T&);

    void validate_mem(Mem mem) const {};
    void validate_handlers() const;
    void validate_connection() const;

    SocketServer& handle_stream(T &mem, int max, int depth);

    SocketServer& get();
    SocketServer& get(int max);
    SocketServer& send(const char* message);

};


#endif //CPPSOCKETS_SOCKETSERVER_H
