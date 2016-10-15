//
// Created by marek on 10/15/16.
//

#ifndef ROUGHDNS_SERVER_H
#define ROUGHDNS_SERVER_H

#include <string>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>

class Server {
private:
    int m_fd;
    struct sockaddr_in m_serverAddr;
    std::string m_mitmAddr;

    void serve();

public:
    Server(std::string ip = "", uint16_t port = 53);
    void listen();
};


#endif //ROUGHDNS_SERVER_H
