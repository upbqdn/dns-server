//
// Created by root on 11/13/16.
//

#include <malloc.h>
#include <stdlib.h>
#include "rr.h"

rr::rr(){

};

rr::rr(std::string name, std::string type, uint32_t ttl, std::string data):
    name(name), type(type), ttl(ttl), rclass("IN"), data(data){

}

std::vector<rr *> rr::parseLdnsZoneFile(ldns_zone * z) {
    std::vector<rr *> rrs;

    rrs = parseLdnsRrList(z->_rrs);
    rrs.push_back(parseLdnsRr(z->_soa));

    return rrs;
}

std::vector<rr *> rr::parseLdnsRrList(ldns_rr_list *ldnsRrs) {
    std::vector<rr *> rrs;

    for (int i = 0; i < ldnsRrs->_rr_count; i++) {
        rrs.push_back(parseLdnsRr(ldnsRrs->_rrs[i]));
    }

    return rrs;
}

rr * rr::parseLdnsRr(ldns_rr * ldnsRr) {
    rr * r = new rr();
    char * tmp;

    tmp = ldns_rdf2str(ldnsRr->_owner);
    r->name = tmp;
    free(tmp);
    r->type = ttos(ldnsRr->_rr_type);
    r->rclass = "IN";
    r->ttl = ldnsRr->_ttl;

    switch (ldnsRr->_rr_type) {
        case LDNS_RR_TYPE_MX:
            tmp = ldns_rdf2str(ldnsRr->_rdata_fields[1]);
            r->data = tmp;
            free(tmp);
            tmp = ldns_rdf2str(*ldnsRr->_rdata_fields);
            r->preference = atoi(tmp);
            free(tmp);
            break;
        case LDNS_RR_TYPE_A:
        case LDNS_RR_TYPE_CNAME:
        case LDNS_RR_TYPE_NS:
        case LDNS_RR_TYPE_PTR:
        case LDNS_RR_TYPE_AAAA:
        case LDNS_RR_TYPE_TXT:
            tmp = ldns_rdf2str(*ldnsRr->_rdata_fields);
            r->data = tmp;
            free(tmp);
            break;
        case LDNS_RR_TYPE_SOA:
            /*fill all the data fields in the soa rr*/
            tmp = ldns_rdf2str(ldnsRr->_rdata_fields[0]);
            r->mname = tmp;
            free(tmp);
            tmp = ldns_rdf2str(ldnsRr->_rdata_fields[1]);
            r->rname = tmp;
            free(tmp);
            tmp = ldns_rdf2str(ldnsRr->_rdata_fields[2]);
            r->serial = atoi(tmp);
            free(tmp);
            tmp = ldns_rdf2str(ldnsRr->_rdata_fields[3]);
            r->refresh = atoi(tmp);
            free(tmp);
            tmp = ldns_rdf2str(ldnsRr->_rdata_fields[4]);
            r->retry = atoi(tmp);
            free(tmp);
            tmp = ldns_rdf2str(ldnsRr->_rdata_fields[5]);
            r->expire = atoi(tmp);
            free(tmp);
            tmp = ldns_rdf2str(ldnsRr->_rdata_fields[6]);
            r->minimum = atoi(tmp);
            free(tmp);
            break;
    }

    return r;
}

std::string rr::ttos(ldns_rr_type t) {
    switch (t) {
        case LDNS_RR_TYPE_A:
            return "A";
        case LDNS_RR_TYPE_CNAME:
            return "CNAME";
        case LDNS_RR_TYPE_SOA:
            return "SOA";
        case LDNS_RR_TYPE_NS:
            return "NS";
        case LDNS_RR_TYPE_PTR:
            return "PTR";
        case LDNS_RR_TYPE_MX:
            return "MX";
        case LDNS_RR_TYPE_TXT:
            return "TXT";
        case LDNS_RR_TYPE_AAAA:
            return "AAAA";
    }
}

const std::string &rr::getName() const {
    return name;
}

const std::string &rr::getType() const {
    return type;
}

const std::string &rr::getRclass() const {
    return rclass;
}

uint32_t rr::getTtl() const {
    return ttl;
}

const std::string &rr::getData() const {
    return data;
}

const std::string &rr::getMname() const {
    return mname;
}

const std::string &rr::getRname() const {
    return rname;
}

uint32_t rr::getSerial() const {
    return serial;
}

uint32_t rr::getRefresh() const {
    return refresh;
}

uint32_t rr::getRetry() const {
    return retry;
}

uint32_t rr::getExpire() const {
    return expire;
}

uint32_t rr::getMinimum() const {
    return minimum;
}

uint16_t rr::getPreference() const {
    return preference;
}

void rr::setPreference(uint16_t p) {
    preference = p;
}
