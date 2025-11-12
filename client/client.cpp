#include "client.h"
#include "lobby/lobby.h"
#include "../protocol/client_protocol.h"

#include <QApplication>
#include <iostream>

Client::Client(const std::string& hostname, const std::string& servname) :
    socket(hostname.c_str(), servname.c_str()),
    protocol(socket),
    player_id(protocol.recv_client_id()),
    lobby_finished(false) {}


void Client::run(int argc, char* argv[]) {
    // FASE 1: lobby (qt)
    QApplication app(argc, argv);

    std::string ip = argv[1];
    std::string port = argv[2];

    Lobby lobby(protocol);

    lobby.show();
    app.exec(); // blocking hasta que se cierre el lobby
    
    std::cout << "[CLIENT] Lobby finished, starting game..." << std::endl;

    // FASE 2: game (SDL)
    GameHandler game_handler(protocol, player_id /*lobby.getCarID()*/);
    game_handler.run(); // blocking hasta que se cierre el juego
    
    std::cout << "[CLIENT] Juego finished" << std::endl;
}

void Client::on_lobby_finished() {
    lobby_finished = true;
    std::cout << "[CLIENT] Signal received: lobby finished." << std::endl;
}

Client::~Client() {
    socket.shutdown(SHUT_RDWR);
    socket.close();
}