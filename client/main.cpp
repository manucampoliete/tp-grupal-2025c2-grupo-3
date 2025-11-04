#include "client.h"
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

/*
int main(int argc, char* argv[]) {
    
    try {
        if (argc != 4) {
            std::cerr << "Bad program call. Expected " << BINARY << " <hostname> <servname>\n";
            return EXIT_FAILURE;
        }

        //    uint8_t my_id = client.get_my_id(); 
    
        const uint8_t my_id = static_cast<uint8_t>(std::stoi(argv[3]));

        Client client;
        client.run(argc, argv);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
*/

int main(int argc, char* argv[]) {
    try {
        if (argc != /*ARGS_COUNT*/ 4) {
            std::cerr << "Bad program call. Expected " << BINARY << " <hostname> <servname>\n";
            return EXIT_FAILURE;
        }

        //    uint8_t my_id = client.get_my_id(); 
    
        const uint8_t my_id = static_cast<uint8_t>(std::stoi(argv[3]));

        Client client(HOSTNAME, SERVNAME); // creo el client, lo conecto con el server


        Game game(client.get_world(), client, my_id); // creo la ventana (game) y lo conecto con el client y con el world que esta adentor de client
        client.set_game(&game); // conecto a client con el game

        client.run(argc, argv); // inicio los hilos del cliente
        game.run();

        client.stop(); // cuando la partida termina detengo al cliente

    } catch (const std::exception& err) {

        syslog(LOG_CRIT, "[Crit] Error!: %s", err.what());
        return EXIT_FAILURE;

    } catch (...) {

        syslog(LOG_CRIT, "[Crit] Unknown error!");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}