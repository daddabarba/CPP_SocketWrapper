//
// Created by daddabarba on 1/12/19.
//

#include "Socket.h"

#include <exception>
#include <stdexcept>

#include <unistd.h>
#include <cstring>

// Constructors

template<typename T> skt::Socket<T>::Mem::BufferData::BufferData(char *&buffer, size_t &buffer_size, int &buffer_max) :
        buffer(buffer),
        buffer_size(buffer_size),
        buffer_max(buffer_max)
{}

template <typename T> skt::Socket<T>::Socket(uint16_t port, int domain, size_t buffer_size) :
        domain(domain),
        handler(nullptr),
        stop_handler(nullptr),
        socket_fd(-1),
        buffer_size(buffer_size),
        buffer((char *)malloc(sizeof(char)*buffer_size)),
        buffer_max(0)
{}

// Socket handler

template<typename T> skt::Socket<T>& skt::Socket<T>::start_handler(T &mem, char *first_message, int max){
    validate_mem(mem);

    mem.data = new typename Socket<T>::Mem::BufferData(this->buffer, this->buffer_size, this->buffer_max);

    start_connection();
    init_stream(first_message);

    return handle_stream(mem, max);
}

template<typename T> skt::Socket<T>& skt::Socket<T>::close_socket() {
    close(socket_fd);
    socket_fd = -1;

    return *this;
}

// Request-response pattern

template<typename T> skt::Socket<T>& skt::Socket<T>::init_stream(char *first_message){

    validate_connection();

    *this << first_message; //Send first communication. To avoid, return nullptr
    *this >> READ_ALL; //Wait for first response

    return *this;
}

template<typename T> skt::Socket<T>& skt::Socket<T>::handle_stream(T &mem, int max){

    validate_handlers();
    validate_connection();

    bool has_to_stop = true;
    int depth = 0;

    do {
        *this << handler(buffer, mem); //compute and send response

        has_to_stop = (max>0 && max<=depth) || stop_handler(buffer, mem);

        if(!has_to_stop)
            *this >> READ_ALL; //wait for response (from other end)

        depth++;

    }while(!has_to_stop);

    return *this;
}

// Stream communication

template<typename T> skt::Socket<T>& skt::Socket<T>::get(){

    if(buffer_max>=buffer_size)
        return *this;

    return get((int)buffer_size-buffer_max);
}

template<typename T> skt::Socket<T>& skt::Socket<T>::get(int max) {

    if(max<=0)
        return get();

    ssize_t n = read(socket_fd, buffer+buffer_max, (size_t)max);
    buffer_max += n;

    if(n<0)
        throw std::runtime_error("ERROR reading socket file desc.");

    return *this;
}

template<typename T> skt::Socket<T>& skt::Socket<T>::send(const char* message){

    if(message == STOP_MESSAGE)
        return *this;

    ssize_t n = write(socket_fd, message, strlen(message));

    if(n<0)
        throw std::runtime_error("ERROR writing in socket file desc.");

    return *this;
}

// Buffer edit methods

template<typename T> char* skt::Socket<T>::get_buffer() {
    return buffer;
}

template<typename T> skt::Socket<T>& skt::Socket<T>::reset_buffer(){
    memset(buffer, 0, buffer_size);
    buffer_max = 0;

    return *this;
}

// Setters

template<typename T> skt::Socket<T>& skt::Socket<T>::set_handler(char* (*handler_function)(char*, T&)){
    this->handler = handler_function;

    return *this;
}

template<typename T> skt::Socket<T>& skt::Socket<T>::set_stop_handler( bool (*stop_handler)(char *, T &)){
    this->stop_handler = stop_handler;

    return *this;
}

// Operators

template<typename T> skt::Socket<T>& skt::Socket<T>::operator>>(std::string& buffer){
    this->get();
    strcpy(this->buffer, buffer.c_str());

    return *this;
}

template<typename T> skt::Socket<T>& skt::Socket<T>::operator>>(int max){
    this->get(max);
    return *this;
}

template<typename T> skt::Socket<T>& skt::Socket<T>::operator<<(const char* message){
    send(message);
    return *this;
}

template<typename T> skt::Socket<T>& skt::Socket<T>::operator<< (std::string const& message){
    send(message.c_str());
    return *this;
}

// Other methods

template<typename T> void skt::Socket<T>::validate_handlers() const{
    if(handler == nullptr || stop_handler == nullptr)
        throw std::runtime_error("handlers are not defined");
}

template<typename T> void skt::Socket<T>::validate_connection() const {
    if(socket_fd < 0)
        throw std::runtime_error("ERROR connection absent");
}

// Destructor

template<typename T> skt::Socket<T>::~Socket() {
    close_socket();
    free(buffer);
}