//
// Created by marek on 10/15/16.
//

#include "server.h"

#define BUFFER_SIZE (512)

Server::Server(std::string mitmAddr, uint16_t port) {
    m_mitmAddr = mitmAddr;

    /*Set the server's properties*/
    memset(&m_serverAddr, 0, sizeof(m_serverAddr));
    m_serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_serverAddr.sin_port = htons(port);
    if ((m_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        throw "could not create the socket";

    if (bind(m_fd, (struct sockaddr*) &m_serverAddr,
             sizeof(m_serverAddr))== -1)
        throw errno;
}

void Server::listen() {
    while(1)
        serve();

    close(m_fd);
}

void Server::serve() {
    int n, r;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in clientAddr;
    socklen_t clientAddrlength = sizeof(clientAddr);

    /*Receive a message and immediately fork a new child.*/
    if ((n = recvfrom(m_fd, buffer, BUFFER_SIZE, 0,
                      (struct sockaddr *) &clientAddr,
                      &clientAddrlength)) >= 0
        && fork() == 0) {

        /* todo: The sleep won't be needed.*/
        sleep(5);

        exit(0);
    } else
        throw"recvfrom() or fork() failed";
}