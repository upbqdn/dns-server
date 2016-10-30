//
// Created by root on 10/23/16.
//

#ifndef ROUGHDNS_DNSMSG_H
#define ROUGHDNS_DNSMSG_H

#include <ios>
#include <vector>
#include <cstring>
#include <iostream>
#include <netinet/in.h>

class dnsMsg {
private:
    struct header {
        /*https://tools.ietf.org/html/rfc1035*/
        uint16_t id;
        char qr : 1;
        char opcode : 4;
        char aa : 1;
        char tc : 1;
        char rd : 1;
        char ra : 1;
        char z : 3;
        char rcode : 4;
        uint16_t qdcount;
        uint16_t ancount;
        uint16_t nscount;
        uint16_t arcount;
    };

    struct question {
        std::string qname;
        std::string qtype;
        std::string qclass;
    };

    struct rr {
        std::string name;
        std::string type;
        std::string rclass;
        int ttl;
        std::string rdata;
    };

    void parseBuffer();
    std::string parseDomainName(uint16_t &i);
    std::string ttos(uint16_t type);
    std::string ctos(uint16_t rclass);

public:
    const static int A = 1;
    const static int SOA = 6;
    const static int MX = 15;
    const static int NS = 2;
    const static int AAAA = 28;
    const static int CNAME = 5;
    const static int PTR = 12;
    const static int TXT = 16;

    const static int IN = 1;

    struct header h;
    std::vector<struct question *> q;
    std::vector<struct question *>::iterator qIt;
    std::vector<struct rr *> ans;
    std::vector<struct rr *>::iterator ansIt;
    std::vector<struct rr *> auth;
    std::vector<struct rr *>::iterator authIt;
    std::vector<struct rr *> add;
    std::vector<struct rr *>::iterator addIt;
    std::vector<char> m_buffer;

    dnsMsg();
    dnsMsg(std::vector<char> buffer);
    ~dnsMsg();

    void init(dnsMsg question);
    std::vector<char> getBuffer();
    void addAnswer(std::string name, std::string rclass, int ttl,
                   std::string data);
};

#endif //ROUGHDNS_DNSMSG_H
