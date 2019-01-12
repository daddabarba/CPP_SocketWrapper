//
// Created by daddabarba on 1/12/19.
//

#include <exception>
#include <stdexcept>

#include <unistd.h>
#include <cstring>

#include "SocketServer.h"


// Constructor

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

void SocketServer::start() {
    socklen_t clilen = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &clilen); // wait for client

    if(client_fd < 0)
        throw std::runtime_error("ERROR on accept");
}

// Server mode

void SocketServer::init(char* (&init_communication)()){
    *this << init_communication(); //Send first communication. To avoid, return nullptr
    get(); //Wait for first response
}

void SocketServer::loop(char* (*handle)(char*), bool (*stop)(char*), int max, int depth){
    loop(handle, stop, DEF_FREE_BUFF, max, depth);
}

void SocketServer::loop(char* (*handle)(char*), bool (*stop)(char*), bool (*free_buff)(char*), int max, int depth){

    *this << handle(buffer); //compute and send response

    bool has_to_stop = stop(buffer);

    if(free_buff(buffer))
        reset_buffer();

    if( !(max>0 && max<=depth) && !has_to_stop) {
        *this >> READ_ALL; //wait for response
        loop(handle, stop, max, depth + 1);
    }
}

// Stream communication

char* SocketServer::get(){

    if(buffer_max>=buffer_size)
        return buffer;

    return get((int)buffer_size-buffer_max);
}

char* SocketServer::get(int max) {

    if(max<=0)
        return get();

    ssize_t n = read(server_fd, buffer+buffer_max, (size_t)max);
    buffer_max += n;

    if(n<0)
        throw std::runtime_error("ERROR reading socket file desc.");

    return buffer;
}

void SocketServer::send(const char* message){

    if(message == STOP_MESSAGE)
        return;

    ssize_t n = write(client_fd, message, strlen(message));

    if(n<0)
        throw std::runtime_error("ERROR writing in socket file desc.");
}

// Buffer edit methods

char* SocketServer::get_buffer() {
    return buffer;
}

void SocketServer::reset_buffer(){
    memset(buffer, 0, buffer_size);
    buffer_max = 0;
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
    close(server_fd);
    close(client_fd);

    free(buffer);
}