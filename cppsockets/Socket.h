//
// Created by daddabarba on 1/12/19.
//

#ifndef CPPSOCKETS_SOCKET_H
#define CPPSOCKETS_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <string>

namespace skt {

    static const int BACKLOG =  5;
    static const int BUFFER_SIZE =  256;

    static const char* STOP_MESSAGE = nullptr;
    static const int READ_ALL = -1;

    //static char* (*DEFAULT_INIT_COMMUNICATIONS)() = [&]()->char*{return nullptr;};

    template<typename T>
    class Socket {

    public:

        // memory struct between receive/send loops
        typedef struct Mem {

            // allow access to buffer's data
            typedef struct BufferData {

                char *&buffer;
                size_t &buffer_size;
                int &buffer_max;

                BufferData(char *&buffer, size_t &buffer_size, int &buffer_max);

            } BufferData;
            BufferData data;
        } Mem;

        //Constructor
        Socket(uint16_t port, int domain, size_t buffer_size);

        // Destructor
        ~Socket();

        // Socket handler
        Socket &start_handler(T &mem, char *first_message = STOP_MESSAGE, int max = -1);

        virtual Socket &start_connection() = 0; //connect to other end of socket
        virtual Socket &close_socket(); //close connection

        Socket &init_stream(char *first_message = STOP_MESSAGE); //send first buffer in stream
        Socket &handle_stream(T &mem, int max); //receive/send loop

        // Setters
        Socket &set_handler(char *(*handler_function)(char *, T &));
        Socket &set_stop_handler( bool (*stop_handler)(char *, T &));

        // Operators
        Socket &operator<<(const char *message); //send buffer
        Socket &operator<<(std::string const &message); //send buffer
        Socket &operator>>(std::string &buffer); //write buffer
        Socket &operator>>(int max); //read "max" characters

        char *get_buffer();

        Socket &reset_buffer();

        // Delete copy from constant reference
        Socket(const Socket &mc) = delete;

        Socket& operator=(const Socket &mc)    = delete;

    protected:

        // Fields

        size_t buffer_size;
        int domain;
        int socket_fd;
        struct sockaddr_in server_addr;

        char *buffer;
        int buffer_max;

        char *(*handler)(char *, T &);
        bool (*stop_handler)(char *, T &);

        // Validation methods

        void validate_mem(Mem mem) const {}; // validate generics's type
        void validate_handlers() const; // validate presence of handlers
        void validate_connection() const; // asses if connection has been established

        Socket &get();
        Socket &get(int max);
        Socket &send(const char *message);

    };
}


#endif //CPPSOCKETS_SOCKETSERVER_H
