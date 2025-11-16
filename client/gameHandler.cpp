#include "gameHandler.h"

#include <iostream>

#include "client.h"
#include "game.h"

#define WORLD_HEIGHT 4672.0f  // Manu's note: used?


GameHandler::GameHandler(ClientProtocol& protocol, ClientID clientId):
    protocol(protocol),
    clientId(clientId),
    world(),
    clientRequestsQueue(),
    serverSnapshotsQueue(),
    sender(protocol, clientRequestsQueue),
    receiver(protocol, serverSnapshotsQueue, gameLoop),  // Manu's note: why gameLoop? Receiver should only interact with the queue
    gameLoop(serverSnapshotsQueue, clientRequestsQueue, protocol, world, clientId),
    running(false) {}


void GameHandler::run() {
    running = true;

    sender.start();
    std::cout << "[GAME_HANDLER] Sender started" << std::endl;

    receiver.start();
    std::cout << "[GAME_HANDLER] Receiver started" << std::endl;

    gameLoop.start();
    std::cout << "[GAME_HANDLER] Gameloop started" << std::endl;

    // Waits for the gameLoop to finish (this happens when the user closes the window)
    gameLoop.join();
    std::cout << "[GAME_HANDLER] GameLoop terminado, cerrando otros threads..." << std::endl;

    stop();
}

void GameHandler::stop() {
    std::cout << "[GAME_HANDLER] Deteniendo..." << std::endl;
    running = false;

    clientRequestsQueue.close();
    serverSnapshotsQueue.close();

    sender.stop();
    receiver.stop();
    gameLoop.stop();  // Manu's note: the thread has been already joined so there's no need to stop it

    sender.join();
    receiver.join();

    std::cout << "[GAME_HANDLER] Stopped." << std::endl;
}


GameHandler::~GameHandler() {
    if (running)
        stop();
}
