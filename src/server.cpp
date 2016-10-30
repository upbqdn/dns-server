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

    m_buffer.resize(BUFFER_SIZE);
    m_mitmAddr = mitmAddr;
    /*Set the server's properties*/
    memset(&m_serverAddr, 0, sizeof(m_serverAddr));
    m_serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_serverAddr.sin_port = htons(port);
}

Server::~Server() {
    close(m_fd);
    delete m_answer;
    delete m_question;
    m_answer = NULL;
    m_question = NULL;
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
    char buffer[BUFFER_SIZE];

    /*Receive a message and immediately fork a new child.*/
    if ((m_ql= recvfrom(m_fd, m_buffer.data(), m_buffer.size(), 0,
                      (struct sockaddr *) &clientAddr,
                      &clientAddrlength)) >= 0) {

        /* todo: swap children and parents */
        if ((p = fork()) == 0) {
            /*parent*/
        } else if (p > 0) {
            /*child*/
            m_buffer.resize(m_ql);
            prepareResponse();
            if (sendto(m_fd, m_buffer.data(), m_buffer.size(), 0,
                       (struct sockaddr *) &clientAddr,
                       clientAddrlength) == -1)
                throw "sendto() failed";
        } else
            throw "fork() failed";

        return p;
    } else
        throw"recvfrom() failed";
}

void Server::prepareResponse() {
    m_answer = new dnsMsg();

    try {
        m_question = new dnsMsg(m_buffer);
    } catch (const char * e) {
        sendErr();
        return;
    }

    m_answer->init(*m_question);
    resolve();


    /* todo change the question to answer*/
    m_buffer = m_question->getBuffer();

}

void Server::resolve() {
    /*iterate through all the questions*/
    for (m_question->qIt = m_question->q.begin();
            m_question->qIt != m_question->q.end();
            m_question->qIt++) {

        int ttl;
        std::string data;

        if ((*m_question->qIt)->qtype == "A") {
            ttl = 3600;
            data = m_mitmAddr;
            //m_answer->addAnswer((*m_question->qIt)->qname, "A", ttl, data);
        }
    }
}

void Server::sendErr() {

}

#pragma clang diagnostic pop