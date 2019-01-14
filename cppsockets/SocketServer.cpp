//
// Created by daddabarba on 1/12/19.
//

#include <exception>
#include <stdexcept>

#include <unistd.h>
#include <cstring>

#include "SocketServer.h"


// Constructors

template<typename T> SocketServer<T>::Mem::BufferData::BufferData(char *&buffer, size_t &buffer_size, int &buffer_max) :
        buffer(buffer),
        buffer_size(buffer_size),
        buffer_max(buffer_max)
{}

template<typename T> SocketServer<T>::SocketServer(uint16_t port,  int domain, size_t buffer_size) :
    handler(nullptr),
    stop_handler(nullptr),
    client_fd(-1),
    buffer_size(buffer_size),
    server_fd(socket(domain, SOCK_STREAM, 0)), // Opening socket
    buffer((char *)malloc(sizeof(char)*buffer_size)),
    buffer_max(0)
{

    if(this->server_fd < 0)
        throw std::runtime_error("ERROR opening socket");

    // Setting socket address
    memset((char *) &server_addr, 0, sizeof(this->server_addr));

    server_addr.sin_family = (sa_family_t)domain;
    server_addr.sin_addr.s_addr = INADDR_ANY; //run on localhost
    server_addr.sin_port = htons(port);

    // Bind socket to address
    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
        throw std::runtime_error("ERROR on binding");

    // Start socket
    listen(server_fd, BACKLOG); // listen at port and address
}

// Socket handler

template<typename T> SocketServer<T>& SocketServer<T>::start_handler(T &mem, char *first_message, int max, int depth){
    validate_mem(mem);

    mem.data = new typename SocketServer<T>::Mem::BufferData(this->buffer, this->buffer_size, this->buffer_max);

    start_connection();
    init_stream(first_message);

    return handle_stream(mem, max, depth);
}

template<typename T> SocketServer<T>& SocketServer<T>::start_connection() {
    socklen_t clilen = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &clilen); // wait for client

    if(client_fd < 0)
        throw std::runtime_error("ERROR on accept");

    return *this;
}

template<typename T> SocketServer<T>& SocketServer<T>::close_socket() {
    close(server_fd);
    close(client_fd);

    return *this;
}

// Request-response pattern

template<typename T> SocketServer<T>& SocketServer<T>::init_stream(char *first_message){

    validate_connection();

    *this << first_message; //Send first communication. To avoid, return nullptr
    *this >> READ_ALL; //Wait for first response

    return *this;
}

template<typename T> SocketServer<T>& SocketServer<T>::handle_stream(T &mem, int max, int depth){

    validate_handlers();
    validate_connection();

    *this << handler(buffer, mem); //compute and send response

    if( !(max>0 && max<=depth) && !stop_handler(buffer, mem)) {
        *this >> READ_ALL; //wait for response
        return handle_stream(mem, max, depth + 1);
    }

    return *this;
}

// Stream communication

template<typename T> SocketServer<T>& SocketServer<T>::get(){

    if(buffer_max>=buffer_size)
        return *this;

    return get((int)buffer_size-buffer_max);
}

template<typename T> SocketServer<T>& SocketServer<T>::get(int max) {

    if(max<=0)
        return get();

    ssize_t n = read(client_fd, buffer+buffer_max, (size_t)max);
    buffer_max += n;

    if(n<0)
        throw std::runtime_error("ERROR reading socket file desc.");

    return *this;
}

template<typename T> SocketServer<T>& SocketServer<T>::send(const char* message){

    if(message == STOP_MESSAGE)
        return *this;

    ssize_t n = write(client_fd, message, strlen(message));

    if(n<0)
        throw std::runtime_error("ERROR writing in socket file desc.");

    return *this;
}

// Buffer edit methods

template<typename T> char* SocketServer<T>::get_buffer() {
    return buffer;
}

template<typename T> SocketServer<T>& SocketServer<T>::reset_buffer(){
    memset(buffer, 0, buffer_size);
    buffer_max = 0;

    return *this;
}

// Setters

template<typename T> SocketServer<T>& SocketServer<T>::set_handler(char* (*handler_function)(char*, T&)){
    this->handler = handler_function;
}

// Operators

template<typename T> SocketServer<T>& SocketServer<T>::operator>>(std::string& buffer){
    this->get();
    strcpy(this->buffer, buffer.c_str());

    return *this;
}

template<typename T> SocketServer<T>& SocketServer<T>::operator>>(int max){
    this->get(max);
    return *this;
}

template<typename T> SocketServer<T>& SocketServer<T>::operator<<(const char* message){
    send(message);
    return *this;
}

template<typename T> SocketServer<T>& SocketServer<T>::operator<< (std::string const& message){
    send(message.c_str());
    return *this;
}

// Other methods

template<typename T> void SocketServer<T>::validate_handlers() const{
    if(handler == nullptr || stop_handler == nullptr)
        throw std::runtime_error("handlers are not defined");
}

template<typename T> void SocketServer<T>::validate_connection() const {
    if(client_fd < 0)
        throw std::runtime_error("ERROR connection absent");
}

// Destructor

template<typename T> SocketServer<T>::~SocketServer() {
    close_socket();
    free(buffer);
}