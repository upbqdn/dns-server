//
// Created by root on 11/13/16.
//

#ifndef ROUGHDNS_RR_H
#define ROUGHDNS_RR_H

#include <iostream>
#include <ldns/ldns.h>
#include <vector>

class rr {
private:
    std::string name;
    std::string type;
    std::string rclass;
    uint32_t ttl;
    std::string data;
    std::string mname;
    std::string rname;
    uint32_t serial;
    uint32_t refresh;
    uint32_t retry;
    uint32_t expire;
    uint32_t minimum;
    uint16_t preference;

    static std::string ttos(ldns_rr_type t);

public:
    const std::string &getName() const;

    const std::string &getType() const;

    const std::string &getRclass() const;

    uint32_t getTtl() const;

    const std::string &getData() const;

    const std::string &getMname() const;

    const std::string &getRname() const;

    uint32_t getSerial() const;

    uint32_t getRefresh() const;

    uint32_t getRetry() const;

    uint32_t getExpire() const;

    uint32_t getMinimum() const;

    uint16_t getPreference() const;

    static std::vector<rr *> parseLdns(ldns_rr_list *ldnsRrs);

    rr(std::string name, std::string type, uint32_t ttl, std::string data);

    rr();
};


#endif //ROUGHDNS_RR_H
