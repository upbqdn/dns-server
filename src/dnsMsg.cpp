//
// Created by root on 10/23/16.
//

#include "dnsMsg.h"

dnsMsg::dnsMsg() {
    m_obuffer.resize(BUFFER_SIZE);
}

dnsMsg::dnsMsg(std::vector<char> buffer) {
    dnsMsg();
    m_buffer = buffer;

    parseBuffer();
}

dnsMsg::~dnsMsg() {
    for (qIt = q.begin(); qIt != q.end(); qIt++) {
        /*todo find out why there's a double free*/
        //delete *qIt;
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
    h.ancount = ans.size();
    h.nscount = auth.size();
    h.arcount = add.size();
    m_obuffer.resize(BUFFER_SIZE);
    loadOutBuffer();
    return m_obuffer;
}

void dnsMsg::loadOutBuffer() {
    /*HEADER*/
    uint8_t tmp8b;
    uint16_t tmp16b;
    uint16_t i = 0;

    tmp16b = htons(h.id);
    memcpy(&m_obuffer[i], &tmp16b, 2); i+=2;

    tmp8b = 0;
    tmp8b = tmp8b | h.qr; tmp8b <<= 4;
    tmp8b = tmp8b | h.opcode; tmp8b <<= 1;
    tmp8b = tmp8b | h.aa; tmp8b <<= 1;
    tmp8b = tmp8b | h.tc; tmp8b <<= 1;
    tmp8b = tmp8b | h.rd;
    memcpy(&m_obuffer[i], &tmp8b, 1); i+=1;

    tmp8b = 0;
    tmp8b = tmp8b | h.ra; tmp8b <<= 3;
    tmp8b = tmp8b | h.z; tmp8b <<= 4;
    tmp8b = tmp8b | h.rcode;
    memcpy(&m_obuffer[i], &tmp8b, 1); i+=1;

    tmp16b = htons(h.qdcount);
    memcpy(&m_obuffer[i], &tmp16b, 2); i+=2;
    tmp16b = htons(h.ancount);
    memcpy(&m_obuffer[i], &tmp16b, 2); i+=2;
    tmp16b = htons(h.nscount);
    memcpy(&m_obuffer[i], &tmp16b, 2); i+=2;
    tmp16b = htons(h.arcount);
    memcpy(&m_obuffer[i], &tmp16b, 2); i+=2;


    /*QUESTION SECTION*/
    insertQuestion(i, q[0]);

    for (ansIt = ans.begin(); ansIt != ans.end(); ansIt++) {
        insertRR(i, **ansIt, true);
    }

    m_obuffer.resize(i);
}

void dnsMsg::insertQuestion(uint16_t &i, const question * q) {
    insertDomainName(i, q->qname);

    std::cout << "q: " << q->qname
              << ": type " << q->qtype
              << ", class " << q->qclass << std::endl;

    uint16_t tmp;

    tmp = htons(stot(q->qtype));
    memcpy(&m_obuffer[i], &tmp, 2); i+=2;
    tmp = htons(stoc(q->qclass));
    memcpy(&m_obuffer[i], &tmp, 2); i+=2;
}

void dnsMsg::insertRR(uint16_t &i, const rr &r, const bool print) {
    insertDomainName(i, r.getName());

    uint16_t tmp16;
    uint32_t tmp32;

    tmp16 = htons(stot(r.getType()));
    memcpy(&m_obuffer[i], &tmp16, 2); i+=2;
    tmp16 = htons(stoc(r.getRclass()));
    memcpy(&m_obuffer[i], &tmp16, 2); i+=2;
    tmp32 = htonl(r.getTtl());
    memcpy(&m_obuffer[i], &tmp32, 4); i+=4;

    if (r.getType() == "A") {
        if (print) {
            std::cout << "r: " << r.getName()
                      << ": type " << r.getType()
                      << ", class " << r.getRclass()
                      << ", addr " << r.getData() << std::endl;
        }

        tmp16 = htons(4);
        memcpy(&m_obuffer[i], &tmp16, 2); i+=2;

        struct sockaddr_in sa;
        inet_pton(AF_INET, r.getData().c_str(), &(sa.sin_addr));
        tmp32 = sa.sin_addr.s_addr;
        memcpy(&m_obuffer[i], &tmp32, 4); i+=4;
    }

}

/*todo: add pointers*/
void dnsMsg::insertDomainName(uint16_t &i, std::string name) {
    std::string d = ".";
    std::string tmp = name;
    size_t ind = 0;
    uint8_t l;

    /*check the last dot in the domain name*/
    if (!tmp.empty())
        if (tmp.back() != '.')
            tmp += '.';

    /*through the whole domain name*/
    while((ind = tmp.find(d)) != std::string::npos) {
        std::string label = tmp.substr(0, ind);

        /*insert the length of the label*/
        l = label.length();
        memcpy(&m_obuffer[i], &l, 1); i+=1;

        /*insert the label itself*/
        for(char & c : label) {
            memcpy(&m_obuffer[i], &c, 1); i += 1;
        }

        tmp.erase(0, ind + d.length());
    }

    /*the terminating zero length byte*/
    l = 0;
    memcpy(&m_obuffer[i], &l, 1); i+=1;
}

/*
void dnsMsg::addRR(section s, std::string name, std::string type,
                   uint32_t ttl, std::string data) {
    rr *r = new rr(name, type, "IN", ttl, data);

    switch (s) {
        case ANS:
            h.ancount++;
            ans.push_back(r);
            break;
        case AUTH:
            h.nscount++;
            auth.push_back(r);
            break;
        case ADD:
            h.arcount++;
            add.push_back(r);
            break;
    }
}

void dnsMsg::addRR(std::string name, std::string type, uint32_t ttl,
                   std::string data) {
    addRR(ANS, name, type, ttl, data);
}

void dnsMsg::addRR(section s, std::string name, std::string type,
                   uint32_t ttl, std::string data, bool aa, uint8_t rcode) {
    addRR(s, name, type, ttl, data);
    h.aa = aa;
    h.rcode = rcode;
}

void dnsMsg::addRR(std::string name, std::string type, uint32_t ttl,
                   std::string data, bool aa, uint8_t rcode) {
    addRR(ANS, name, type, ttl, data, aa, rcode);
}
*/

void dnsMsg::setAA(bool aa) {
    h.aa = aa;
}

void dnsMsg::setRcode(uint8_t c) {
    h.rcode = c;
}

void dnsMsg::parseBuffer() {
    uint8_t tmp;
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
    h.id = ntohs(h.id);
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
    h.qdcount = ntohs(h.qdcount);
    memcpy(&(h.ancount), &m_buffer[i], 2); i+=2;
    h.ancount = ntohs(h.ancount);
    memcpy(&(h.nscount), &m_buffer[i], 2); i+=2;
    h.nscount = ntohs(h.nscount);
    memcpy(&(h.arcount), &m_buffer[i], 2); i+=2;
    h.arcount = ntohs(h.arcount);

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
        tmp = ntohs(tmp);
        qp->qtype = ttos(tmp);

        memcpy(&tmp, &m_buffer[i], 2); i+=2;
        tmp = ntohs(tmp);
        qp->qclass = ctos(tmp);

        q.push_back(qp);
    }

    /*todo: finish the rest of the packet*/
}

const std::string dnsMsg::parseDomainName(uint16_t &i) {
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
            for (uint8_t j = m_buffer[i]; j > 0; j--)
                name += m_buffer[++i];

            if (m_buffer[++i] != 0)
                name += ".";
            else
                break;
        }
    }

    i++;

    return name;
}

void dnsMsg::init(const dnsMsg & question) {
    h.id = question.h.id;
    h.qr = 1;
    h.opcode = question.h.opcode;
    h.tc = 0;
    h.rd = question.h.rd;
    h.ra = 1;
    h.z = 0;
    h.qdcount = question.h.qdcount;
    h.ancount = 0;
    h.nscount = 0;
    h.arcount = 0;

    q = question.q;
}

/*type to string*/
const std::string dnsMsg::ttos(uint16_t type) {
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

/*string to type*/
uint16_t dnsMsg::stot(std::string type) {
    if (type == "A")
        return A;
    else if (type == "SOA")
        return SOA;
    else if (type == "MX")
        return MX;
    else if (type == "NS")
        return NS;
    else if (type == "AAAA")
        return AAAA;
    else if (type == "CNAME")
        return CNAME;
    else if (type == "PTR")
        return PTR;
    else if (type == "TXT")
        return TXT;
    else
        return 0;

}

/*string to class*/
uint16_t dnsMsg::stoc(std::string rclass) {
    if (rclass == "IN")
        return IN;
    else
        return 0;
}

/*class to string*/
const std::string dnsMsg::ctos(uint16_t rclass) {
    switch(rclass) {
        case IN:
            return "IN";
        default:
            return "ERROR";
    }
}