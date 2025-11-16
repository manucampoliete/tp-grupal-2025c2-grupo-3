#include "client.h"

#include <QApplication>
#include <iostream>

#include "../protocol/client_protocol.h"
#include "lobby/lobby.h"


Client::Client(const std::string& hostname, const std::string& servname):
        socket(hostname.c_str(), servname.c_str()),
        protocol(socket),
        clientId(protocol.recv_client_id()),
        lobbyFinished(false) {}


void Client::run(int argc, char* argv[]) {
    // Phase 1: Lobby (Qt)
    QApplication app(argc, argv);

    Lobby lobby(protocol);

    lobby.show();
    app.exec();  // Blocks until lobby is closed

    if (!lobby.shouldStartGame()) {
        std::cout << "[CLIENT] Lobby closed, exiting client..." << std::endl;
        return;
    }

    std::cout << "[CLIENT] Lobby finished, starting game..." << std::endl;

    // Phase 2: Game (SDL)
    GameHandler gameHandler(protocol, clientId);
    gameHandler.run();  // Blocks until game is closed

    std::cout << "[CLIENT] Game finished" << std::endl;
}

void Client::onLobbyFinished() {
    lobbyFinished = true;
    std::cout << "[CLIENT] Signal received: lobby finished." << std::endl;
}

Client::~Client() {
    socket.shutdown(SHUT_RDWR);
    socket.close();
}
