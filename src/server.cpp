//
// Created by marek on 10/15/16.
//

#include <csignal>
#include <iostream>
#include <cstdlib>
#include "server.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "CannotResolve"

int Server::m_fd;

Server::Server(std::string mitmAddr, uint16_t port) {

    m_mitmAddr = mitmAddr;
    /*Set the server's properties*/
    memset(&m_serverAddr, 0, sizeof(m_serverAddr));
    m_serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_serverAddr.sin_port = htons(port);
}

Server::~Server() {
    close(m_fd);
}

void Server::listen() {

    if ((m_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        throw "could not create the socket";

    if (bind(m_fd, (struct sockaddr*) &m_serverAddr,
             sizeof(m_serverAddr))== -1)
        throw errno;

    /*ignore all the dying children*/
    signal(SIGCHLD, SIG_IGN);

    while(serve());
}


int Server::serve() {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrlength = sizeof(clientAddr);
    pid_t p;

    /*Receive a message and immediately fork a new child.*/
    if ((m_ql= recvfrom(m_fd, m_buffer, BUFFER_SIZE, 0,
                      (struct sockaddr *) &clientAddr,
                      &clientAddrlength)) >= 0) {
        if ((p = fork()) > 0) {
            /*parent*/
        } else if (p == 0) {
            /*child*/
            prepareResponse();
            if (sendto(m_fd, m_buffer, m_rl, 0,
                       (struct sockaddr *) &clientAddr,
                       clientAddrlength) == -1)
                throw "sendto() failed";
            sleep(10);
        } else
            throw "fork() failed";

        return p;
    } else
        throw"recvfrom() failed";
}

void Server::prepareResponse() {

}

#pragma clang diagnostic pop