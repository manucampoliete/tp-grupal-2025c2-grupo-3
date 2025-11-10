#include <exception>
#include <iostream>
#include <string>

#include <syslog.h>

#include "lobby/lobby.h"

#include "client.h"
#include "game.h"

#define ARGS_COUNT 3
#define BINARY argv[0]
#define HOSTNAME argv[1]
#define SERVNAME argv[2]

/**
 * Main function: creates and runs the client.
 * Expects exactly two arguments: the server hostname and service name (to connect to).
 * Catches and logs all exceptions, returning EXIT_FAILURE in that case.
 * Returns EXIT_SUCCESS if everything went fine.
 */
int main(int argc, char* argv[]) {
    try {
        if (argc != ARGS_COUNT) {
            std::cerr << "Bad program call. Expected " << BINARY << " <hostname> <servname>\n";
            return EXIT_FAILURE;
        }

        Client client(HOSTNAME, SERVNAME);
        client.run(argc, argv);

    } catch (const std::exception& err) {

        syslog(LOG_CRIT, "[Crit] Error!: %s", err.what());
        return EXIT_FAILURE;

    } catch (...) {

        syslog(LOG_CRIT, "[Crit] Unknown error!");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
