#include "gameResolver.h"

GameResolver::GameResolver(ClientID clientId, Queue<std::unique_ptr<Command>>& clientCommandsQueue):
        clientId(clientId), clientCommandsQueue(clientCommandsQueue) {}

void GameResolver::handleMove(const ActiveDirections& activeDirections) {
    clientCommandsQueue.push(std::make_unique<MoveCommand>(clientId, activeDirections));
}
