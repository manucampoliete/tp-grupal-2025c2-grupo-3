#include "game_handler.h"

#include <iostream>

#include "client.h"
#include "game.h"

#define WORLD_HEIGHT 4672.0f


GameHandler::GameHandler(ClientProtocol& protocol, uint8_t player_id):
        protocol(protocol),
        player_id(player_id),
        world(),
        client_requests_q(),
        server_snapshots_q(),
        sender(protocol, client_requests_q),
        receiver(protocol, server_snapshots_q, game_loop),
        game_loop(server_snapshots_q, client_requests_q, protocol, world, player_id),
        running(false) {}


void GameHandler::run() {
    running = true;

    sender.start();
    std::cout << "[GAME_HANDLER] Sender iniciado" << std::endl;

    receiver.start();
    std::cout << "[GAME_HANDLER] Receiver iniciado" << std::endl;

    game_loop.start();
    std::cout << "[GAME_HANDLER] Gameloop iniciado" << std::endl;

    // esperar a que el GameLoop termine (cuando el usuario cierra la ventana)
    game_loop.join();
    std::cout << "[GAME_HANDLER] GameLoop terminado, cerrando otros threads..." << std::endl;

    stop();
}

void GameHandler::stop() {
    std::cout << "[GAME_HANDLER] Deteniendo..." << std::endl;
    running = false;

    client_requests_q.close();
    server_snapshots_q.close();

    sender.stop();
    receiver.stop();
    game_loop.stop();

    sender.join();
    receiver.join();

    std::cout << "[GAME_HANDLER] Stopped." << std::endl;
}


GameHandler::~GameHandler() {
    if (running)
        stop();
}
