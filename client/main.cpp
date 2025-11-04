#include "client.h"
#include <QApplication>
#include "lobby/lobby.h"

#include <iostream>
#include <exception>
#include <iostream>
#include <string>
#include <syslog.h>

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
        Client client;
        client.run(argc, argv);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
