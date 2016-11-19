//
// Created by marek on 10/14/16.
//

#include <stdlib.h>
#include "argParser.h"

ArgParser::ArgParser() {

}

ArgParser::~ArgParser() {
    for (std::vector<rr *>::iterator i = m_zoneFile.begin();
         i < m_zoneFile.end(); i++) {
        delete *i;
    }
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

    /*parse the zone files*/
    while (optind < argc) {
        FILE *fp = NULL;
        ldns_zone *z;
        int line_nr = 0;
        ldns_status s;

        if (NULL == (fp = fopen(argv[optind++], "r")))
            throw std::invalid_argument("could not open the zone file");

        if (LDNS_STATUS_OK != ldns_zone_new_frm_fp_l(&z, fp, NULL, 0,
                                                              LDNS_RR_CLASS_IN,
                                                              &line_nr))
            throw std::invalid_argument("could not parse the zone file");

        std::vector<rr *> tmp = rr::parseLdnsZoneFile(z);
        m_zoneFile.insert(m_zoneFile.end(), tmp.begin(), tmp.end());
        ldns_zone_deep_free(z);
    }
}

void ArgParser::processPort(std::string port) {
    char* p;
    m_port = strtol(port.c_str(), &p, 10);

    if (*p != 0 || m_port < 0 || m_port > 65535)
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

std::vector<rr *>ArgParser::getZone() const {
    return m_zoneFile;
}