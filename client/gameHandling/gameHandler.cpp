#include "gameHandler.h"
#include "game.h"

#include <iostream>

GameHandler::GameHandler(Socket& skt, ClientID clientId):
    clientId(clientId),
    clientCommandQueue(),
    serverMessagesQueue(),
    sender(skt, clientCommandQueue),
    receiver(skt, serverMessagesQueue),
    gameLoop(serverMessagesQueue, clientCommandQueue, clientId),
    running(false) {}


void GameHandler::run() {
    running = true;

    sender.start();

    receiver.start();

    gameLoop.start();

    // Waits for the gameLoop to finish (this happens when the user closes the window)
    gameLoop.join();

    stop();
}

void GameHandler::stop() {
    running = false;

    /**
     * Manu:
     * At a given point of the project Queue::close() was modified to not throw exception when already closed.
     * This allows to do this close() calls without try-catch.
     * But maybe this is not a good idea, we can revert this change if needed.
     */
    clientCommandQueue.close();
    serverMessagesQueue.close();

    sender.stop();
    receiver.stop();

    sender.join();
    receiver.join();
}


GameHandler::~GameHandler() {
    if (running) stop();
}
