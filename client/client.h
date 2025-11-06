#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <memory>

#include "../common/socket/socket.h"
#include "../common/protocol/client_protocol.h"
#include "game_handler.h"
#include <sys/socket.h>

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
    uint8_t player_id;  // por ahora hardcodeado, después lo asigna el servidor
    
    bool lobby_finished;

public:
    /**
     * Constructor
     * hostname: IP o hostname del servidor
     * servname: Puerto del servidor
     * player_id: ID del jugador (temporal, hardcodeado)
     */
    Client(const std::string& hostname, const std::string& servname, uint8_t player_id);

    /**
     * ejecuta el flujo completo: lanza el lobby (Qt), espera a que termine y lanza el GameHandler (SDL)
     */
    void run(int argc, char* argv[]);
    
    /**
     * llamado cuando el lobby termina
     */
    void on_lobby_finished();
    
    ~Client();
};

#endif // CLIENT_H