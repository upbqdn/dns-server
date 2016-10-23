//
// Created by marek on 10/15/16.
//

#ifndef ROUGHDNS_SERVER_H
#define ROUGHDNS_SERVER_H

#include <string>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>

#include "dnsMsg.h"

class Server {
private:

    static int m_fd;
    /*query and response length*/
    ssize_t m_ql;
    size_t m_rl;
    struct sockaddr_in m_serverAddr;
    std::string m_mitmAddr;
    const static uint BUFFER_SIZE = 512;
    char m_buffer[BUFFER_SIZE];
    dnsMsg m_question;
    dnsMsg m_answer;

    int serve();
    void prepareResponse();

public:
    Server(std::string ip = "", uint16_t port = 53);
    ~Server();
    void listen();
};


#endif //ROUGHDNS_SERVER_H
