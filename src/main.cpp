#include "main.h"

int main(int argc, char** argv)
{
    ArgParser argParser;

    try {
        argParser.parse(argc, argv);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Wrong arguments - " << e.what();
        return EXIT_FAILURE;
    } catch (const std::domain_error &e) {
        std::cout << helpMsg;
        return EXIT_SUCCESS;
    }


    try {
        Server server(argParser.getIP(), argParser.getPort());
        server.listen();
    } catch (int e) {
        std::cerr << e;
    }

    return EXIT_SUCCESS;
}