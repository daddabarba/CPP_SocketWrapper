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

        typedef struct Mem {
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

        // Methods
        Socket &start_handler(T &mem, char *first_message = STOP_MESSAGE, int max = -1, int depth = 0);

        virtual Socket &start_connection() = 0;
        Socket &close_socket();

        Socket &init_stream(char *first_message = STOP_MESSAGE);

        //Setters
        Socket &set_handler(char *(*handler_function)(char *, T &));

        //Operators
        Socket &operator<<(const char *message);
        Socket &operator<<(std::string const &message);
        Socket &operator>>(std::string &buffer);
        Socket &operator>>(int max);

        char *get_buffer();

        Socket &reset_buffer();

        //Delete copy from constant reference
        Socket(const Socket &mc) = delete;

        Socket& operator=(const Socket &mc)    = delete;

    protected:
        size_t buffer_size;
        int socket_fd;
        struct sockaddr_in server_addr;

        char *buffer;
        int buffer_max;

        char *(*handler)(char *, T &);
        bool (*stop_handler)(char *, T &);

        void validate_mem(Mem mem) const {};
        void validate_handlers() const;
        void validate_connection() const;

        Socket &handle_stream(T &mem, int max, int depth);

        Socket &get();
        Socket &get(int max);
        Socket &send(const char *message);

    };
}


#endif //CPPSOCKETS_SOCKETSERVER_H
