//
// Created by marek on 10/14/16.
//

#ifndef ROUGHDNS_ARGPARSER_H
#define ROUGHDNS_ARGPARSER_H

#include <getopt.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <arpa/inet.h>


class ArgParser {
private:
    const struct option long_options[4] = {
            {"mitm", required_argument, 0, 'm'},
            {"help", no_argument,       0, 'h'},
            {"port", required_argument, 0, 'p'},
            {0,      0,                 0, 0}
    };

    const char* short_options = "m:hp:";
    int m_port = 53;
    std::string m_ip = "";
    std::string m_zoneFile = "";

    void processPort(std::string port);
    void processIP(std::string IP);

public:
    ArgParser();
    ~ArgParser();
    void parse(int argc, char** argv);
    int getPort() const;
    std::string getIP() const;
    std::string getZoneFile() const;
};

#endif //ROUGHDNS_ARGPARSER_H
