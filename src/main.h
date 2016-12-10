//
// Created by marek on 10/14/16.
//

#ifndef ROUGHDNS_MAIN_H
#define ROUGHDNS_MAIN_H

#include <iostream>
#include <stdlib.h>
#include <stdexcept>
#include "argParser.h"
#include "server.h"
#include <ldns/ldns.h>

const std::string helpMsg = ""
"RoughDNS is a recursive, concurrent DNS server, which supports various resource\n"
"record types and runs over UDP on Unix-like systems. It is written in C++\n"
"according to RFC 1035 and it also supports multiple zone files (utilizing\n"
"an open source library - https://www.nlnetlabs.nl/projects/ldns/).\n"
"Supported resource records: A, AAAA, SOA, MX, NS, TXT, PTR and CNAME.\n\n"

        "USAGE:\n"
"./roughDNS [-m <ip_addr>] [-h] [-p <port#>] [<zonefile1>] [<zonefileX>]\n\n"

"-m [--mitm] <ip_address>\n"
"Responds to every A and MX query with the given IP address. This parameter\n"
"simulates the man in the middle attack. Other records are served\n"
"in the standard way.\n\n"

"-h [--help]\n"
"Prints out a simple user manual.\n\n"

"-p [--port] <port number>\n"
"The server will bind its socket with the given port number. If not specified,\n"
"the standard port number will be used - 53.\n\n"

"The last optional parameters are the paths to zone files. The resource records\n"
"specified in the zone files must be unique.\n";
void destroy();
void sigIntHandler(int s);
#endif //ROUGHDNS_MAIN_H
