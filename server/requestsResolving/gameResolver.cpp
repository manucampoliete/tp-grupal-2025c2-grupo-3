#include "gameResolver.h"

#include "../commands/inmortalityCommand.h"
#include "../commands/instaLoseCommand.h"
#include "../commands/instaWinCommand.h"
#include "../commands/moveCommand.h"
#include "../commands/improvementsCommand.h"
#include "../commands/superSpeedCommand.h"
#include "../commands/disconnectPlayerCommand.h"

GameResolver::GameResolver(ClientID clientId, Queue<std::unique_ptr<Command>>& clientCommandsQueue):
        clientId(clientId), clientCommandsQueue(clientCommandsQueue) {}

void GameResolver::handleMove(const ActiveDirections& activeDirections) {
    clientCommandsQueue.push(std::make_unique<MoveCommand>(clientId, activeDirections));
}

void GameResolver::handleInmortality() {
    clientCommandsQueue.push(std::make_unique<InmortalityCommand>(clientId));
}

void GameResolver::handleInstaWin() {
    clientCommandsQueue.push(std::make_unique<InstaWinCommand>(clientId));
}

void GameResolver::handleInstaLose() {
    clientCommandsQueue.push(std::make_unique<InstaLoseCommand>(clientId));
}

void GameResolver::handleSuperSpeed() {
    clientCommandsQueue.push(std::make_unique<SuperSpeedCommand>(clientId));
}

void GameResolver::handleModifyCar(bool improveVelocity, bool improveHealth) {
    clientCommandsQueue.push(std::make_unique<ImprovementsCommand>(clientId, improveVelocity, improveHealth));
}

void GameResolver::handlePlayerDisconnected() {
    clientCommandsQueue.push(std::make_unique<DisconnectPlayerCommand>(clientId));
}
