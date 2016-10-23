//
// Created by root on 10/23/16.
//

#ifndef ROUGHDNS_DNSMSG_H
#define ROUGHDNS_DNSMSG_H

#include <ios>
#include <vector>

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
        uint16_t qtype;
        uint16_t qclass;
    };

    struct rr {
        std::string name;
        uint16_t type;
        uint16_t rclass;
        uint32_t ttl;
        uint16_t rdlength;
        std::string rdata;
    };

public:
    struct header h;
    std::vector<struct question> question;
    std::vector<struct rr> answer;
    std::vector<struct rr> authority;
    std::vector<struct rr> additional;
    char* buffer;

    dnsMsg();
    ~dnsMsg();
};

#endif //ROUGHDNS_DNSMSG_H
