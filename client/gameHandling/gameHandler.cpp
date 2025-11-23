#include "gameHandler.h"
#include "game.h"

#include <iostream>

GameHandler::GameHandler(Socket& skt, ClientID clientId):
    protocol(skt),
    clientId(clientId),
    clientCommandQueue(),
    serverMessagesQueue(),
    sender(protocol, clientCommandQueue),
    receiver(protocol, serverMessagesQueue),
    gameLoop(serverMessagesQueue, clientCommandQueue, clientId),
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
    std::cout << "[GAME_HANDLER] GameLoop finished, stopping other threads..." << std::endl;

    stop();
}

void GameHandler::stop() {
    std::cout << "[GAME_HANDLER] Stopping..." << std::endl;
    running = false;

    clientCommandQueue.close();
    serverMessagesQueue.close();

    sender.stop();
    receiver.stop();

    sender.join();
    receiver.join();

    std::cout << "[GAME_HANDLER] Stopped." << std::endl;
}


GameHandler::~GameHandler() {
    if (running) stop();
}
