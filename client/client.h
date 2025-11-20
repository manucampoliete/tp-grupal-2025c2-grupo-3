#ifndef CLIENT_H
#define CLIENT_H

#include <memory>
#include <string>

#include <sys/socket.h>

#include "../common/messages/snapshot.h"
#include "../common/socket/socket.h"

#include "gameHandler.h"


/**
 * Client: connect to the server and coordinate Lobby → Game
 * - Owner of the socket and protocol
 * - Launches the lobby (Qt)
 * - When the lobby ends, launches the GameHandler (SDL)
 */
class Client {
private:
    Socket skt;
    ClientID clientId;
    bool lobbyFinished;

public:
    /**
     * Constructor
     * hostname: server IP/hostname
     * servname: server port
     */
    Client(const std::string& hostname, const std::string& servname);

    /**
     * Executes the complete flow: launches the lobby (Qt), waits for it to finish, and then launches the GameHandler
     * (SDL)
     */
    void run(int argc, char* argv[]);

    /**
     * Called when lobby finishes
     */
    void onLobbyFinished();

    ~Client();
};

#endif  // CLIENT_H
