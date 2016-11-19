#include <stdlib.h>
#include "main.h"

ArgParser* argParser;
Server* server;

int main(int argc, char** argv)
{
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
        std::cout << helpMsg << std::endl;
        return EXIT_SUCCESS;
    }

    server = new Server(argParser->getZone(), argParser->getIP(),
                        argParser->getPort());

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
    std::cerr << "\nBye." << std::endl;
    _exit(EXIT_SUCCESS);
}

