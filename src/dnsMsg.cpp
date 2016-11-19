//
// Created by root on 10/23/16.
//

#include "dnsMsg.h"

dnsMsg::dnsMsg() {
    m_obuffer.resize(BUFFER_INIT_SIZE);
}

dnsMsg::dnsMsg(std::vector<char> buffer) {
    dnsMsg();
    m_buffer = buffer;

    parseBuffer();
}

dnsMsg::~dnsMsg() {
    for(pIt = p.begin(); pIt < p.end(); pIt++) {
        delete *pIt;
    }

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
    h.ancount = ans.size();
    h.nscount = auth.size();
    h.arcount = add.size();
    //m_obuffer.resize(BUFFER_INIT_SIZE);
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

    /*ANSWER SECTION*/
    for (ansIt = ans.begin(); ansIt != ans.end(); ansIt++)
        insertRR(i, **ansIt, true);

    /*AUTHORITY SECTION*/
    for (authIt = auth.begin(); authIt != auth.end(); authIt++)
        insertRR(i, **authIt, false);

    /*ADDITIONAL SECTION*/
    for (addIt = add.begin(); addIt != add.begin(); addIt++)
        insertRR(i, **addIt, false);

    /*check the truncation*/
    if(i > BUFFER_MAX_SIZE) {
        m_obuffer.resize(BUFFER_MAX_SIZE);
        h.tc = true;
    } else {
        m_obuffer.resize(i);
    }
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

        /*TYPE A*/
    if (r.getType() == "A") {
        if (print) stdOutPrint(A, r);

        tmp16 = htons(4);
        memcpy(&m_obuffer[i], &tmp16, 2); i+=2;

        struct sockaddr_in sa;
        inet_pton(AF_INET, r.getData().c_str(), &(sa.sin_addr));
        tmp32 = sa.sin_addr.s_addr;
        memcpy(&m_obuffer[i], &tmp32, 4); i+=4;
    }
        /*TYPE MX*/
    else if (r.getType() == "MX") {
        if (print) stdOutPrint(MX, r);
        uint16_t sizePos = i;
        i+=2;

        tmp16 = htons(r.getPreference());
        memcpy(&m_obuffer[i], &tmp16, 2); i+=2;

        /*the length of this rdata field is the length of the name +
         * the length of the preference which is 2*/
        tmp16 = htons(insertDomainName(i, r.getData()) + 2);
        memcpy(&m_obuffer[sizePos], &tmp16, 2);
    }
        /*TYPE SOA*/
    else if (r.getType() == "SOA") {
        if (print) stdOutPrint(SOA, r);
        uint16_t sizePos = i;
        i+=2;
        insertDomainName(i, r.getMname());
        insertDomainName(i, r.getRname());
        tmp32 = htonl(r.getSerial());
        memcpy(&m_obuffer[i], &tmp32, 4); i+=4;
        tmp32 = htonl(r.getRefresh());
        memcpy(&m_obuffer[i], &tmp32, 4); i+=4;
        tmp32 = htonl(r.getRetry());
        memcpy(&m_obuffer[i], &tmp32, 4); i+=4;
        tmp32 = htonl(r.getExpire());
        memcpy(&m_obuffer[i], &tmp32, 4); i+=4;
        tmp32 = htonl(r.getMinimum());
        memcpy(&m_obuffer[i], &tmp32, 4); i+=4;
        tmp16 = htons(i - sizePos - 2);
        memcpy(&m_obuffer[sizePos], &tmp16, 2);

    }
        /*TYPE AAAA*/
    else if (r.getType() == "AAAA") {
        if (print) stdOutPrint(AAAA, r);
        tmp16 = htons(16);
        memcpy(&m_obuffer[i], &tmp16, 2); i+=2;

        struct sockaddr_in6 sa;
        inet_pton(AF_INET6, r.getData().c_str(), &(sa.sin6_addr));
        memcpy(&m_obuffer[i], &(sa.sin6_addr.__in6_u), 16); i+=16;
    }
        /*TYPE CNAME*/
    else if (r.getType() == "CNAME") {
        if (print) stdOutPrint(CNAME, r);
        uint16_t sizePos = i;
        i+=2;
        tmp16 = htons(insertDomainName(i, r.getData()));
        memcpy(&m_obuffer[sizePos], &tmp16, 2);
    }
        /*TYPE NS*/
    else if (r.getType() == "NS") {
        if (print) stdOutPrint(NS, r);
        uint16_t sizePos = i;
        i+=2;
        tmp16 = htons(insertDomainName(i, r.getData()));
        memcpy(&m_obuffer[sizePos], &tmp16, 2);
    }
        /*TYPE PTR*/
    else if (r.getType() == "PTR") {
        if (print) stdOutPrint(PTR, r);
        uint16_t sizePos = i;
        i+=2;
        tmp16 = htons(insertDomainName(i, r.getData()));
        memcpy(&m_obuffer[sizePos], &tmp16, 2);
    }
}

void dnsMsg::stdOutPrint(uint16_t type, rr r) {
    switch (type) {
        case A:
            std::cout << "r: " << r.getName()
                      << ": type " << r.getType()
                      << ", class " << r.getRclass()
                      << ", addr " << r.getData() << std::endl;
            break;
        case CNAME:
            std::cout << "r: " << r.getName()
                      << ": type " << r.getType()
                      << ", class " << r.getRclass()
                      << ", cname " << r.getData() << std::endl;
            break;
        case NS:
            std::cout << "r: " << r.getName()
                      << ": type " << r.getType()
                      << ", class " << r.getRclass()
                      << ", ns " << r.getData() << std::endl;
            break;
        case PTR:
            std::cout << "r: " << r.getName()
                      << ": type " << r.getType()
                      << ", class " << r.getRclass()
                      << r.getData() << std::endl;
            break;
        case SOA:
            std::cout << "r: " << r.getName()
                      << ": type " << r.getType()
                      << ", class " << r.getRclass()
                      << ", mname " << r.getMname() << std::endl;
            break;
        case MX:
            std::cout << "r: " << r.getName()
                      << ": type " << r.getType()
                      << ", class " << r.getRclass()
                      << ", preference " << r.getPreference()
                      << ", mx " << r.getData() << std::endl;
           break;
        case TXT:
            std::cout << "r: " << r.getName()
                      << ": type " << r.getType()
                      << ", class " << r.getRclass()
                      << ", txt \"" << r.getData() << "\"" << std::endl;
            break;
        case AAAA:
            std::cout << "r: " << r.getName()
                      << ": type " << r.getType()
                      << ", class " << r.getRclass()
                      << ", addr " << r.getData() << std::endl;
           break;
    }
}

/*returns the number of written bytes*/
uint16_t dnsMsg::insertDomainName(uint16_t &i, std::string name) {
    uint16_t iBefore = i;
    std::string d = ".";
    uint16_t p;

    /*if we're at the end of the name, write the terminating zero byte*/
    if(name.empty()) {
        uint8_t l = 0;
        memcpy(&m_obuffer[i], &l, 1); i+=1;
        return 1;
    }

    /*check the last dot in the domain name*/
    if (name.back() != '.')
        name += '.';

    if(p = getPointer(name)) {
        /*we've got a pointer, write it to the buffer*/
        p |= 0b1100000000000000;
        p = htons(p);
        memcpy(&m_obuffer[i], &p, 2); i+=2;
        /*the pointer takes two bytes in the buffer*/
        return 2;
    } else {
        /*let's write the first part of the name*/
        addPointer(name, i);
        std::string chunk = name.substr(0, name.find(d));

        /*write the length of the chunk*/
        uint8_t l = chunk.length();
        memcpy(&m_obuffer[i], &l, 1); i+=1;

        /*write chunk itself*/
        for (char & c : chunk) {
            memcpy(&m_obuffer[i], &c, 1); i += 1;
        }

        std::string tail = name.erase(0, name.find(d) + d.length());
        return l + 1 + insertDomainName(i, tail);
    }
}

uint16_t dnsMsg::getPointer(std::string name) {
    for (pIt = p.begin(); pIt < p.end(); pIt++){
        if((*pIt)->name == name)
            return (*pIt)->i;
        else
            return 0;
    }
}

void dnsMsg::addPointer(std::string name, uint16_t i) {
    struct pointer * point = new struct pointer;
    point->i = i;
    point->name = name;
    p.push_back(point);
}

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

void dnsMsg::init(dnsMsg & question) {
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

    for (qIt = question.q.begin(); qIt < question.q.end(); qIt++) {
        struct question * quest = new struct question;
        *quest = **qIt;
        q.push_back(quest);
    }
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