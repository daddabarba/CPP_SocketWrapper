//
// Created by daddabarba on 1/12/19.
//

#include "Socket.h"

#include <unistd.h>
#include <cstring>

// MEM CLASS

// Constructors

skt::BufferData::BufferData(EditSocket *socket, size_t *buffer_size, int *buffer_max) :
        socket_interface(socket),
        buffer_size(buffer_size),
        buffer_max(buffer_max)
{}

// HANDLER CLASS

// Use handlers

auto skt::Handler::handle(char* buffer) -> char * {
    return this->handler_function(buffer, this->mem);
};

auto skt::Handler::has_to_stop(char* buffer) -> bool {
    return this->stop_handler(buffer, this->mem);
};

// Validation

auto skt::Handler::validate_handlers() -> void {
    // validate presence of handlers

    validate_mem(this->mem);

    if(handler_function == nullptr || stop_handler == nullptr)
        throw std::runtime_error("handlers are not defined");
};

// Setters

auto skt::Handler::set_handler_function(skt::Handler::Handler_fct handler_function) -> skt::Handler& {
    this->handler_function = handler_function;
    return *this;
};

auto skt::Handler::set_stop_handler( skt::Handler::Stop_fct stop_handler) -> skt::Handler& {
    this->stop_handler = stop_handler;
    return *this;
};

auto skt::Handler::set_data(BufferData data) -> skt::Handler& {
    this->mem = data;
    return *this;
}

// Getters

auto skt::Handler::get_handler_function() -> skt::Handler::Handler_fct {
    return handler_function;
};

auto skt::Handler::get_stop_handler() -> skt::Handler::Stop_fct {
    return stop_handler;
};

// SOCKET CLASS

// Constructors

skt::Socket::Socket(int domain, size_t buffer_size) :
        handler(),
        domain(domain),
        socket_fd(socket(this->domain, SOCK_STREAM, 0)),
        buffer_size(buffer_size),
        buffer((char *)malloc(sizeof(char)*buffer_size)),
        buffer_max(0)
{
    if(this->socket_fd < 0)
        throw std::runtime_error("ERROR opening socket");
}

// Socket handler

auto skt::Socket::start_handler(char const* first_message, int max) -> skt::Socket& {
    this->handler.validate_handlers();

    skt::BufferData *data = new skt::BufferData(this, &(this->buffer_size), &(this->buffer_max));
    this->handler.set_data(*(data));

    start_connection();
    init_stream(first_message);

    return handle_stream(max);
}

auto skt::Socket::close_socket() -> skt::Socket& {
    close(socket_fd);
    socket_fd = -1;

    return *this;
}

// Request-response pattern

auto skt::Socket::init_stream(const char *first_message) -> skt::Socket& {

    validate_connection();

    *this << first_message; //Send first communication. To avoid, return nullptr
    *this >> READ_ALL; //Wait for first response

    return *this;
}

auto skt::Socket::handle_stream(int max) -> skt::Socket& {

    this->handler.validate_handlers();
    validate_connection();

    bool has_to_stop;
    int depth = 0;

    do {
        *this << this->handler.handle(buffer); //compute and send response

        has_to_stop = (max>0 && max<=depth) || this->handler.has_to_stop(buffer);

        if(!has_to_stop)
            *this >> READ_ALL; //wait for response (from other end)

        depth++;

    }while(!has_to_stop);

    return *this;
}

// Stream communication

auto skt::Socket::get() -> skt::Socket& {

    if(buffer_max>=buffer_size)
        return *this;

    return get((int)buffer_size-buffer_max);
}

auto skt::Socket::get(int max) -> skt::Socket& {

    if(max<=0)
        return get();

    ssize_t n = read(socket_fd, buffer+buffer_max, (size_t)max);
    buffer_max += n;

    if(n<0)
        throw std::runtime_error("ERROR reading socket file desc.");

    return *this;
}

auto skt::Socket::send(const char* message) -> skt::Socket& {

    if(message == STOP_MESSAGE)
        return *this;

    ssize_t n = write(socket_fd, message, strlen(message));

    if(n<0)
        throw std::runtime_error("ERROR writing in socket file desc.");

    return *this;
}

// Buffer edit methods

auto skt::Socket::get_buffer() -> char* {
    return buffer;
}

auto skt::Socket::reset_buffer() -> skt::Socket& {
    memset(buffer, 0, buffer_size);
    buffer_max = 0;
}

// Setters

auto skt::Socket::set_handler(skt::Handler& handler) -> skt::Socket& {
    this->handler = handler;

    return *this;
}

// Operators

auto skt::Socket::operator>>(std::string& buffer) -> skt::Socket& {
    this->get();

    buffer.clear();
    buffer.append(this->buffer);

    return *this;
}

auto skt::Socket::operator>>(int max) -> skt::Socket& {
    this->get(max);
    return *this;
}

auto skt::Socket::operator<<(const char* message) -> skt::Socket& {
    send(message);
    return *this;
}

auto skt::Socket::operator<< (std::string const& message) -> skt::Socket& {
    send(message.c_str());
    return *this;
}

// Other methods

auto skt::Socket::validate_connection() -> void {
    if(socket_fd < 0)
        throw std::runtime_error("ERROR connection absent");
}

// Destructor

skt::Socket::~Socket() {
    close_socket();
    free(buffer);
}