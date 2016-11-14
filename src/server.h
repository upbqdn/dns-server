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
#include <csignal>
#include <iostream>
#include <resolv.h>
#include <netdb.h>
#include <ldns/ldns.h>
#include "rr.h"
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
    const static uint32_t TTL = 3600;
    std::vector<char> m_buffer;
    dnsMsg* m_question = NULL;
    dnsMsg* m_answer = NULL;

    int serve();
    void prepareResponse();
    void resolve();
    void remoteResolve(question q, ldns_rr_type t);
    void sendErr();

public:
    Server(std::string ip = "", uint16_t port = 53);
    ~Server();
    void listen();
};


#endif //ROUGHDNS_SERVER_H
