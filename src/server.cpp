//
// Created by marek on 10/15/16.
//

#include "server.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "CannotResolve"

int Server::m_fd;

Server::Server(std::string mitmAddr, uint16_t port) {

    m_buffer.resize(BUFFER_SIZE);
    m_mitmAddr = mitmAddr;
    /*Set the server's properties*/
    memset(&m_serverAddr, 0, sizeof(m_serverAddr));
    m_serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_serverAddr.sin_port = htons(port);
}

Server::~Server() {
    close(m_fd);
    delete m_answer;
    delete m_question;
    m_answer = NULL;
    m_question = NULL;
}

void Server::listen() {

    if ((m_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        throw "could not create the socket";

    if (bind(m_fd, (struct sockaddr*) &m_serverAddr,
             sizeof(m_serverAddr))== -1)
        throw errno;

    /*ignore all the dying children*/
    signal(SIGCHLD, SIG_IGN);

    while(serve());
}

int Server::serve() {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrlength = sizeof(clientAddr);
    pid_t p;
    char buffer[BUFFER_SIZE];

    /*Receive a message and immediately fork a new child.*/
    if ((m_ql= recvfrom(m_fd, m_buffer.data(), m_buffer.size(), 0,
                      (struct sockaddr *) &clientAddr,
                      &clientAddrlength)) >= 0) {

        /* todo: swap children and parents */
        if ((p = fork()) > 0) {
            /*parent*/
        } else if (p == 0) {
            /*child*/
            m_buffer.resize(m_ql);
            prepareResponse();
            if (sendto(m_fd, m_buffer.data(), m_buffer.size(), 0,
                       (struct sockaddr *) &clientAddr,
                       clientAddrlength) == -1)
                throw "sendto() failed";
        } else
            throw "fork() failed";

        return p;
    } else
        throw"recvfrom() failed";
}

void Server::prepareResponse() {
    m_answer = new dnsMsg();

    try {
        m_question = new dnsMsg(m_buffer);
    } catch (const char * e) {
        sendErr();
        return;
    }

    m_answer->init(*m_question);
    resolve();
    m_buffer = m_answer->getBuffer();
}

void Server::resolve() {
    uint ttl;
    std::string data;
    bool aa;
    uint8_t rcode;
    const question q = *m_question->q[0];
    ldns_resolver *res;
    ldns_rdf *d;
    ldns_pkt *p;
    ldns_rr_list *rrs;
    ldns_status s;

    d = ldns_dname_new_frm_str(q.qname.c_str());
    s = ldns_resolver_new_frm_file(&res, NULL);

    if (q.qtype == "A") {
        if (!m_mitmAddr.empty()) {
            rr * r = new rr(q.qname, "A", TTL, m_mitmAddr);
            m_answer->ans.push_back(r);
            m_answer->setAA(true);
            m_answer->setRcode(dnsMsg::OK);
        } else {
            p = ldns_resolver_query(res, d, LDNS_RR_TYPE_A,
                                    LDNS_RR_CLASS_IN, LDNS_RD);
            rrs = ldns_pkt_answer(p);

            m_answer->ans = rr::parseLdns(rrs);

            //std::cout << "hel" << std::endl;
            //ldns_rr_list_free(rrs);
            //ldns_pkt_free(p);
            //ldns_resolver_deep_free(res);
        }
    }

}

void Server::sendErr() {

}

#pragma clang diagnostic pop