//
// Created by daddabarba on 1/12/19.
//

#include <exception>
#include <stdexcept>

#include <unistd.h>
#include <cstring>

#include "SocketServer.h"


// Constructors

SocketServer::Mem::BufferData::BufferData(char *&buffer, size_t &buffer_size, int &buffer_max) :
        buffer(buffer),
        buffer_size(buffer_size),
        buffer_max(buffer_max)
{}

SocketServer::SocketServer(uint16_t port,  int domain, size_t buffer_size) :
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

SocketServer& SocketServer::start_socket() {
    socklen_t clilen = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &clilen); // wait for client

    if(client_fd < 0)
        throw std::runtime_error("ERROR on accept");

    return *this;
}

SocketServer& SocketServer::close_socket() {
    close(server_fd);
    close(client_fd);

    return *this;
}

// Server mode

SocketServer& SocketServer::init(char* (*init_communication)()){
    *this << init_communication(); //Send first communication. To avoid, return nullptr
    *this >> READ_ALL; //Wait for first response

    return *this;
}

template<typename T> SocketServer& SocketServer::loop(char* (*handle)(char*, T&), bool (*stop)(char*, T&), T& mem, int max, int depth){
    validate_mem(mem);

    mem.data = new SocketServer::Mem::BufferData(this->buffer, this->buffer_size, this->buffer_max);
    return loop_aux(handle, stop, mem, max, depth);
}

template<typename T> SocketServer& SocketServer::loop_aux(char* (*handle)(char*, T&), bool (*stop)(char*, T&), T& mem, int max, int depth){

    *this << handle(buffer, mem); //compute and send response

    if( !(max>0 && max<=depth) && !stop(buffer, mem)) {
        *this >> READ_ALL; //wait for response
        return loop_aux(handle, stop, mem, max, depth + 1);
    }

    return *this;
}

// Stream communication

SocketServer& SocketServer::get(){

    if(buffer_max>=buffer_size)
        return *this;

    return get((int)buffer_size-buffer_max);
}

SocketServer& SocketServer::get(int max) {

    if(max<=0)
        return get();

    ssize_t n = read(client_fd, buffer+buffer_max, (size_t)max);
    buffer_max += n;

    if(n<0)
        throw std::runtime_error("ERROR reading socket file desc.");

    return *this;
}

SocketServer& SocketServer::send(const char* message){

    if(message == STOP_MESSAGE)
        return *this;

    ssize_t n = write(client_fd, message, strlen(message));

    if(n<0)
        throw std::runtime_error("ERROR writing in socket file desc.");

    return *this;
}

// Buffer edit methods

char* SocketServer::get_buffer() {
    return buffer;
}

SocketServer& SocketServer::reset_buffer(){
    memset(buffer, 0, buffer_size);
    buffer_max = 0;

    return *this;
}

// Operators

SocketServer& SocketServer::operator>>(std::string& buffer){
    this->get();
    strcpy(this->buffer, buffer.c_str());

    return *this;
}

SocketServer& SocketServer::operator>>(int max){
    this->get(max);
    return *this;
}

SocketServer& SocketServer::operator<<(const char* message){
    send(message);
    return *this;
}

SocketServer& SocketServer::operator<< (std::string const& message){
    send(message.c_str());
    return *this;
}

// Destructor

SocketServer::~SocketServer() {
    close_socket();
    free(buffer);
}