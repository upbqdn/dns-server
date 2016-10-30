//
// Created by root on 10/23/16.
//

#include "dnsMsg.h"

dnsMsg::dnsMsg() {
}

dnsMsg::dnsMsg(std::vector<char> buffer) {
    m_buffer = buffer;

    parseBuffer();
    for (qIt = q.begin(); qIt != q.end(); qIt++) {
        std::cout << "q: " << (*qIt)->qname
                  << ": type " << (*qIt)->qtype
                  << ", class " << (*qIt)->qclass << std::endl;
    }
}

dnsMsg::~dnsMsg() {
    for (qIt = q.begin(); qIt != q.end(); qIt++) {
        delete *qIt;
    }

    for (ansIt = ans.begin(); ansIt != ans.end(); ansIt++) {
        delete *ansIt;
    }

    for (authIt = auth.begin(); authIt != auth.end(); authIt++) {
        delete *authIt;
    }

    for (addIt = add.begin(); addIt != add.end(); addIt++) {
        delete *addIt;
    }
}

std::vector<char> dnsMsg::getBuffer() {
    return m_buffer;
}

void dnsMsg::addAnswer(std::string name, std::string rclass, int ttl,
                       std::string data) {

}

void dnsMsg::parseBuffer() {
    char tmp;
    uint16_t i = 0;
    /* parse the header */
    /*https://tools.ietf.org/html/rfc1035
                                     1  1  1  1  1  1
       0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |                      ID                       |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |                    QDCOUNT                    |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |                    ANCOUNT                    |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |                    NSCOUNT                    |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |                    ARCOUNT                    |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     */
    memcpy(&(h.id), &m_buffer[i], 2); i+=2;
    h.id = htons(h.id);
    memcpy(&tmp, &m_buffer[i], 1); i+=1;
    h.qr = tmp &     0b10000000;
    h.opcode = tmp & 0b01111000;
    h.aa = tmp &     0b00000100;
    h.tc = tmp &     0b00000010;
    h.rd = tmp &     0b00000001;
    memcpy(&tmp, &m_buffer[i], 1); i+=1;
    h.ra = tmp &     0b10000000;
    h.z = tmp &      0b01110000;
    h.rcode = tmp &  0b00001111;
    memcpy(&(h.qdcount), &m_buffer[i], 2); i+=2;
    h.qdcount = htons(h.qdcount);
    memcpy(&(h.ancount), &m_buffer[i], 2); i+=2;
    h.ancount = htons(h.ancount);
    memcpy(&(h.nscount), &m_buffer[i], 2); i+=2;
    h.nscount = htons(h.nscount);
    memcpy(&(h.arcount), &m_buffer[i], 2); i+=2;
    h.arcount = htons(h.arcount);

    if (h.qr != 0 ||
            h.opcode != 0 ||
            h.tc != 0 ||
            h.z != 0 ||
            h.rcode != 0 ||
            h.ancount !=0)
        throw "invalid query";

    /* parse questions */
    /*                              1  1  1  1  1  1
      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                                               |
    /                     QNAME                     /
    /                                               /
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     QTYPE                     |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     QCLASS                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     */
    for (uint16_t j = h.qdcount; j > 0; j--) {
        question *qp = new question;
        qp->qname = parseDomainName(i);

        uint16_t tmp;

        memcpy(&tmp,&m_buffer[i], 2); i+=2;
        tmp = htons(tmp);
        qp->qtype = ttos(tmp);

        memcpy(&tmp, &m_buffer[i], 2); i+=2;
        tmp = htons(tmp);
        qp->qclass = ctos(tmp);

        q.push_back(qp);
    }
}

std::string dnsMsg::parseDomainName(uint16_t &i) {
    std::string name;

    while (1) {
        if (m_buffer[i] & 0b1100000) {
            /* we got a pointer */
            uint16_t j;
            memcpy(&j, &m_buffer[i], 2); i+=1;
            j = j & 0b0011111111111111;
            name += parseDomainName(j);
            break;
        } else {
            /* we got a sequence of labels */
            for (uint8_t j = m_buffer[i]; j > 0; j--) {
                name += m_buffer[++i];
            }

            if (m_buffer[++i] != 0)
                name += ".";
            else
                break;
        }
    }

    i++;

    return name;
}

void dnsMsg::init(dnsMsg question) {
    h.id = question.h.id;
    /*todo: finish the rest of the header */
}

std::string dnsMsg::ttos(uint16_t type) {
    switch(type) {
        case A:
            return "A";
        case SOA:
            return "SOA";
        case MX:
            return "MX";
        case NS:
            return "NS";
        case AAAA:
            return "AAAA";
        case CNAME:
            return "CNAME";
        case PTR:
            return "PTR";
        case TXT:
            return "TXT";
        default:
            return "ERROR";
    }
}

std::string dnsMsg::ctos(uint16_t rclass) {
    switch(rclass) {
        case IN:
            return "IN";
        default:
            return "ERROR";
    }
}