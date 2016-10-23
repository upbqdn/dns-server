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

const std::string helpMsg = "help yourself";
void destroy();
void sigIntHandler(int s);
#endif //ROUGHDNS_MAIN_H
