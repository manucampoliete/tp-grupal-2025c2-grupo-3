#include <exception>
#include <iostream>
#include <string>

#include <syslog.h>

#include "server.h"

#define ARGS_COUNT 2
#define BINARY argv[0]
#define SERVNAME argv[1]

/**
 * Main function: creates and runs the server.
 * Expects exactly one argument: the service name (to bind the socket to).
 * Catches and logs all exceptions, returning EXIT_FAILURE in that case.
 * Returns EXIT_SUCCESS if everything went fine.
 */
int main(int argc, char* argv[]) {
    try {

        if (argc != ARGS_COUNT) {
            std::cerr << "Bad program call. Expected " << BINARY << " <servname>\n";
            return EXIT_FAILURE;
        }

        return Server(std::string(SERVNAME)).run();

    } catch (const std::exception& err) {

        syslog(LOG_CRIT, "[Crit] Error!: %s", err.what());
        return EXIT_FAILURE;

    } catch (...) {

        syslog(LOG_CRIT, "[Crit] Unknown error!");
        return EXIT_FAILURE;
    }
}
