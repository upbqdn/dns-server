//
// Created by root on 11/13/16.
//

#include "rr.h"

rr::rr(){

};

rr::rr(std::string name, std::string type, uint32_t ttl, std::string data):
    name(name), type(type), ttl(ttl), rclass("IN"), data(data){

}

std::vector<rr *> rr::parseLdns(ldns_rr_list * ldnsRrs) {
    std::vector<rr *> rrs;

    for (int i = 0; i < ldnsRrs->_rr_count; i++) {
        ldns_rr * ldnsRr = ldnsRrs->_rrs[i];
        rr * r = new rr();

        r->name = ldns_rdf2str(ldnsRr->_owner);
        r->type = ttos(ldnsRr->_rr_type);
        r->rclass = "IN";
        r->ttl = ldnsRr->_ttl;

        switch (ldnsRr->_rr_type) {
            case LDNS_RR_TYPE_A:
                r->data = ldns_rdf2str(*ldnsRr->_rdata_fields);
        }
        rrs.push_back(r);
    }

    return rrs;
}

std::string rr::ttos(ldns_rr_type t) {
    switch (t) {
        case LDNS_RR_TYPE_A:
            return "A";
        case LDNS_RR_TYPE_SOA:
            return "SOA";

        /*todo: finnish the rest*/
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
