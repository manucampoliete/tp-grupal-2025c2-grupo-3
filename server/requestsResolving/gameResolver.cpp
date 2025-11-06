#include "gameResolver.h"

GameResolver::GameResolver(ClientID clientId, Queue<std::unique_ptr<Command>>& clientCommandsQueue)
    : client_id(clientId), clientCommandsQueue(clientCommandsQueue) {}

void GameResolver::handleMove(const ActiveDirections& directions) {
    clientCommandsQueue.push(std::make_unique<MoveCommand>(client_id, directions));
}
