#include "moveCommand.h"

MoveCommand::MoveCommand(ClientID clientId, ActiveDirections activeDirections):
    Command(clientId), activeDirections(activeDirections) {}

void MoveCommand::execute(Game& game) {
    game.movePlayer(getClientID(), activeDirections);
}