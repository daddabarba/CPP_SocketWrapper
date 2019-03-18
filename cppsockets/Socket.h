//
// Created by daddabarba on 1/12/19.
//

#ifndef CPPSOCKETS_SOCKET_H
#define CPPSOCKETS_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <stdexcept>

#include <string>
#include <form.h>

namespace skt {

    static const int BACKLOG =  5;
    static const size_t BUFFER_SIZE =  256;

    static const char* STOP_MESSAGE = nullptr;
    static const int READ_ALL = -1;

    class EditSocket;
    class Socket;

    // memory struct between receive/send loops
    class Mem {

    public:

        // allow access to buffer's data
        class BufferData {

        public:

            // Fields
            EditSocket *socket_interface;

            size_t *buffer_size;
            int *buffer_max;

            // Constructors
            BufferData(EditSocket *socket, size_t *buffer_size, int *buffer_max);
            BufferData() = default;

        };

        // Fields
        BufferData data;
        void *obj;

        // Constructor
        explicit Mem(void *obj= nullptr);

        // Setters
        Mem &set_obj(void *obj);
    };

    class Handler{

    public:


        typedef char *(*Handler_Function)(char *, Mem &);
        typedef bool (*Stop_Handler)(char *, Mem &);

        // Construtor
        Handler() = default;

        // Destructor
        ~Handler() = default;

        // Use handlers
        char *handle(char* buffer);
        bool has_to_stop(char* buffer);

        // Validation
        void validate_mem(Mem mem) const {}; // validate generics's type

        void validate_handlers();

        // Setters
        Handler &set_handler_function(char *(*handler_function)(char *, Mem &));
        Handler &set_stop_handler( bool (*stop_handler)(char *, Mem &));

        Handler &set_mem(Mem &mem);
        Handler &set_data(Mem::BufferData data);

        // Getters
        Handler_Function get_handler_function();
        Stop_Handler get_stop_handler();

    protected:
        char *(*handler_function)(char *, Mem &);
        bool (*stop_handler)(char *, Mem &);

        Mem mem;
    };

    class EditSocket{
    public:
        virtual Socket &reset_buffer() = 0;

    };

    class Socket : public EditSocket{

    public:

        // Constructor
        Socket(uint16_t port, int domain, size_t buffer_size);

        // Destructor
        ~Socket();

        // Socket handler
        Socket &start_handler(char const* first_message = STOP_MESSAGE, int max = -1);

        virtual Socket &start_connection() = 0; //connect to other end of socket
        virtual Socket &close_socket(); //close connection

        Socket &init_stream(const char *first_message = STOP_MESSAGE); //send first buffer in stream
        Socket &handle_stream(int max); //receive/send loop

        // Setters
        Socket &set_handler(skt::Handler& handler);

        // Operators
        Socket &operator<<(const char *message); //send buffer
        Socket &operator<<(std::string const &message); //send buffer
        Socket &operator>>(std::string &buffer); //write buffer
        Socket &operator>>(int max); //read "max" characters

        char *get_buffer();

        Socket &reset_buffer() override;

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

        Handler handler;

        // Validation methods

        void validate_connection(); // asses if connection has been established

        Socket &get();
        Socket &get(int max);
        Socket &send(const char *message);

    };
}


#endif //CPPSOCKETS_SOCKETSERVER_H
