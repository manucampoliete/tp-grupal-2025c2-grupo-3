#include "client.h"

#include <QApplication>
#include <iostream>

#include "lobby/lobby.h"
#include "../protocol/clientHandshakeProtocol.h"


Client::Client(const std::string& hostname, const std::string& servname):
        skt(hostname.c_str(), servname.c_str()),
        clientId(ClientHandshakeProtocol(skt).recvClientId()),
        lobbyFinished(false) {}


void Client::run(int argc, char* argv[]) {
    // Phase 1: Lobby (Qt)
    QApplication app(argc, argv);

    Lobby lobby(skt);

    lobby.show();
    app.exec();  // Blocks until lobby is closed

    if (!lobby.shouldStartGame()) {
        return;
    }

    // Phase 2: Game (SDL)
    GameHandler gameHandler(skt, clientId);
    gameHandler.run();  // Blocks until game is closed
}

void Client::onLobbyFinished() {
    lobbyFinished = true;
}

Client::~Client() {
    // Manu: this shouldn't be necessary, Socket's destructor should handle it.
    skt.shutdown(SHUT_RDWR);
    skt.close();
}
