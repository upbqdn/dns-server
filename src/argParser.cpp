//
// Created by marek on 10/14/16.
//

#include "argParser.h"
#include <iostream>
#include <stdexcept>
#include <arpa/inet.h>

ArgParser::ArgParser() {

}

void ArgParser::parse(int argc, char **argv){
    int c;
    int option_index = 0;

    while((c = getopt_long(argc, argv, short_options, long_options,
                           &option_index)) != -1) {
        switch (c) {
            case 'p':
                if (optarg)
                    processPort(optarg);
                break;

            case 'h':
                throw std::domain_error("help message");

            case 'm':
                if (optarg)
                processIP(optarg);
                break;

            case '?':
            default:
                throw std::invalid_argument("wrong syntax");
        }

    }
}

void ArgParser::processPort(std::string port) {
    char* p;
    m_port = strtol(port.c_str(),&p, 10);

    if (*p != 0)
        throw std::invalid_argument("port number is not valid");
}

int ArgParser::getPort() const {
    return m_port;
}

void ArgParser::processIP(std::string IP) {
    struct sockaddr_in addr;
    if (!inet_pton(AF_INET, IP.c_str(), &(addr.sin_addr)))
        throw std::invalid_argument("IP address is not valid");

    m_ip = IP;
}

std::string ArgParser::getIP() const {
    return m_ip;
}