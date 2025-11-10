#ifndef CLIENT_H
#define CLIENT_H

#include <memory>
#include <string>

#include <sys/socket.h>

#include "../common/messages/snapshot.h"
#include "../common/socket/socket.h"
#include "../protocol/client_protocol.h"

#include "game_handler.h"

/**
 * Client: conectar con el servidor y coordinar Lobby → Game
 * - Dueño del socket y protocol
 * - Lanza el lobby (Qt)
 * - Cuando el lobby termina, lanza el GameHandler (SDL)
 */
class Client {
private:
    Socket socket;
    ClientProtocol protocol;
    uint8_t player_id;

    bool lobby_finished;

public:
    /**
     * Constructor
     * hostname: IP o hostname del servidor
     * servname: Puerto del servidor
     * player_id: ID del jugador (temporal, hardcodeado)
     */
    Client(const std::string& hostname, const std::string& servname);

    /**
     * ejecuta el flujo completo: lanza el lobby (Qt), espera a que termine y lanza el GameHandler
     * (SDL)
     */
    void run(int argc, char* argv[]);

    /**
     * llamado cuando el lobby termina
     */
    void on_lobby_finished();

    ~Client();
};

#endif  // CLIENT_H
