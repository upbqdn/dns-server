#include <stdlib.h>
#include "main.h"

ArgParser* argParser;
Server* server;

int main(int argc, char** argv)
{
    /*********************************************************************/
    /*test code*/
    char *filename;
    FILE *fp;
    ldns_zone *z;
    int line_nr = 0;
    int c;
    bool canonicalize = false;
    bool sort = false;
    bool strip = false;
    bool only_dnssec = false;
    bool print_soa = true;
    ldns_status s;
    /*********************************************************************/

    signal(SIGINT, sigIntHandler);

    argParser = new ArgParser;

    try {
        argParser->parse(argc, argv);
    } catch (const std::invalid_argument& e) {
        destroy();
        std::cerr << "Wrong arguments - " << e.what();
        return EXIT_FAILURE;
    } catch (const std::domain_error &e) {
        destroy();
        std::cout << helpMsg;
        return EXIT_SUCCESS;
    }

    /*********************************************************************/
    /*test code*/
    fp = fopen(argParser->getZoneFile().c_str(), "r");
    if (fp)
        s = ldns_zone_new_frm_fp_l(&z, fp, NULL, 0, LDNS_RR_CLASS_IN, &line_nr);
    /*********************************************************************/

    server = new Server(argParser->getIP(), argParser->getPort());

    try {
        server->listen();
    } catch (const int& e) {
        std::cerr << e << std::endl;
        destroy();
        return EXIT_FAILURE;
    } catch (const char*& err) {
        std::cerr << err << std::endl;
        destroy();
        return EXIT_FAILURE;
    }

    destroy();
    return EXIT_SUCCESS;
}

void destroy() {
    delete argParser;
    argParser = NULL;
    delete server;
    server = NULL;
}

void sigIntHandler(int s) {
    destroy();
    std::cerr << "Bye." << std::endl;
    _exit(EXIT_SUCCESS);
}

