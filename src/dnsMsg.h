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
#include <arpa/inet.h>
#include "rr.h"

struct header {
    /*https://tools.ietf.org/html/rfc1035*/
    uint16_t id;
    uint8_t qr : 1;
    uint8_t opcode : 4;
    uint8_t aa : 1;
    uint8_t tc : 1;
    uint8_t rd : 1;
    uint8_t ra : 1;
    uint8_t z : 3;
    uint8_t rcode : 4;
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

class dnsMsg {
private:
    struct pointer {
        std::string name;
        uint16_t i;
    };

    const static uint8_t QUERY = 0;
    const static uint8_t RESPONSE = 1;
    const static uint8_t IQUERY = 1;

    const static int BUFFER_SIZE = 512;

    std::vector<char> m_buffer;
    std::vector<char> m_obuffer;
    std::vector<pointer *> pointers;

    void parseBuffer();
    const std::string parseDomainName(uint16_t &i);
    uint16_t insertDomainName(uint16_t &i, std::string name);
    void insertQuestion(uint16_t &i, const question * q);
    void insertRR(uint16_t &i, const rr & r, const bool print);
    void stdOutPrint(uint16_t type, rr r);
    void loadOutBuffer();
    uint16_t getPointer(std::string name);
    void addPointer(std::string name, uint16_t i);

public:
    const static uint16_t A = 1;
    const static uint16_t SOA = 6;
    const static uint16_t MX = 15;
    const static uint16_t NS = 2;
    const static uint16_t AAAA = 28;
    const static uint16_t CNAME = 5;
    const static uint16_t PTR = 12;
    const static uint16_t TXT = 16;

    const static uint16_t IN = 1;

    const static uint8_t OK = 0;

    enum section {ANS, AUTH, ADD};

    struct header h;
    std::vector<struct pointer *> p;
    std::vector<struct pointer *>::iterator pIt;
    std::vector<struct question *> q;
    std::vector<struct question *>::iterator qIt;
    std::vector<rr *> ans;
    std::vector<rr *>::iterator ansIt;
    std::vector<rr *> auth;
    std::vector<rr *>::iterator authIt;
    std::vector<rr *> add;
    std::vector<rr *>::iterator addIt;

    dnsMsg();
    dnsMsg(std::vector<char> buffer);
    ~dnsMsg();

    void init(dnsMsg & question);
    std::vector<char> getBuffer();
    void setAA(bool aa);
    void setRcode(uint8_t c);

    static const std::string ttos(uint16_t type);
    static const std::string ctos(uint16_t rclass);
    static uint16_t stot(std::string type);
    static uint16_t stoc(std::string rclass);
};

#endif //ROUGHDNS_DNSMSG_H
