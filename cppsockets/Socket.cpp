//
// Created by daddabarba on 1/12/19.
//

#include "Socket.h"

#include <unistd.h>
#include <cstring>

// MEM CLASS

// Constructors

skt::Mem::Mem(void *obj) :
        data(),
        obj(obj)
{}

skt::Mem::BufferData::BufferData(EditSocket *socket, size_t *buffer_size, int *buffer_max) :
        socket_interface(socket),
        buffer_size(buffer_size),
        buffer_max(buffer_max)
{}

// Setters

skt::Mem& skt::Mem::set_obj(void *obj) {
    this->obj = obj;

    return *this;
}

// HANDLER CLASS

// Use handlers

char * skt::Handler::handle(char* buffer){
    return this->handler_function(buffer, this->mem);
};

bool skt::Handler::has_to_stop(char* buffer){
    return this->stop_handler(buffer, this->mem);
};

// Validation

void skt::Handler::validate_handlers() const {
    // validate presence of handlers

    validate_mem(this->mem);

    if(handler_function == nullptr || stop_handler == nullptr)
        throw std::runtime_error("handlers are not defined");
};

// Setters

skt::Handler &skt::Handler::set_handler_function(char *(*handler_function)(char *, Mem &)){
    this->handler_function = handler_function;
    return *this;
};

skt::Handler &skt::Handler::set_stop_handler( bool (*stop_handler)(char *, Mem &)){
    this->stop_handler = stop_handler;
    return *this;
};

skt::Handler &skt::Handler::set_mem(Mem &mem){
    this->mem = mem;
    return *this;
};

skt::Handler &skt::Handler::set_data(Mem::BufferData data) {
    this->mem.data = data;
    return *this;
}

// Getters

skt::Handler::Handler_Function skt::Handler::get_handler_function(){
    return handler_function;
};

skt::Handler::Stop_Handler skt::Handler::get_stop_handler(){
    return stop_handler;
};

// SOCKET CLASS

// Constructors

skt::Socket::Socket(uint16_t port, int domain, size_t buffer_size) :
        handler(),
        domain(domain),
        socket_fd(-1),
        buffer_size(buffer_size),
        buffer((char *)malloc(sizeof(char)*buffer_size)),
        buffer_max(0)
{}

// Socket handler

skt::Socket& skt::Socket::start_handler(char const* first_message, int max){
    this->handler.validate_handlers();

    skt::Mem::BufferData *data = new skt::Mem::BufferData(this, &(this->buffer_size), &(this->buffer_max));
    this->handler.set_data(*(data));

    start_connection();
    init_stream(first_message);

    return handle_stream(max);
}

skt::Socket& skt::Socket::close_socket() {
    close(socket_fd);
    socket_fd = -1;

    return *this;
}

// Request-response pattern

skt::Socket& skt::Socket::init_stream(const char *first_message){

    validate_connection();

    *this << first_message; //Send first communication. To avoid, return nullptr
    *this >> READ_ALL; //Wait for first response

    return *this;
}

skt::Socket& skt::Socket::handle_stream(int max){

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

skt::Socket& skt::Socket::get(){

    if(buffer_max>=buffer_size)
        return *this;

    return get((int)buffer_size-buffer_max);
}

skt::Socket& skt::Socket::get(int max) {

    if(max<=0)
        return get();

    ssize_t n = read(socket_fd, buffer+buffer_max, (size_t)max);
    buffer_max += n;

    if(n<0)
        throw std::runtime_error("ERROR reading socket file desc.");

    return *this;
}

skt::Socket& skt::Socket::send(const char* message){

    if(message == STOP_MESSAGE)
        return *this;

    ssize_t n = write(socket_fd, message, strlen(message));

    if(n<0)
        throw std::runtime_error("ERROR writing in socket file desc.");

    return *this;
}

// Buffer edit methods

char* skt::Socket::get_buffer() {
    return buffer;
}

skt::Socket& skt::Socket::reset_buffer(){
    memset(buffer, 0, buffer_size);
    buffer_max = 0;
}

// Setters

skt::Socket& skt::Socket::set_handler(skt::Handler& handler){
    this->handler = handler;

    return *this;
}

// Operators

skt::Socket& skt::Socket::operator>>(std::string& buffer){
    this->get();
    strcpy(this->buffer, buffer.c_str());

    return *this;
}

skt::Socket& skt::Socket::operator>>(int max){
    this->get(max);
    return *this;
}

skt::Socket& skt::Socket::operator<<(const char* message){
    send(message);
    return *this;
}

skt::Socket& skt::Socket::operator<< (std::string const& message){
    send(message.c_str());
    return *this;
}

// Other methods

void skt::Socket::validate_connection() const {
    if(socket_fd < 0)
        throw std::runtime_error("ERROR connection absent");
}

// Destructor

skt::Socket::~Socket() {
    close_socket();
    free(buffer);
}