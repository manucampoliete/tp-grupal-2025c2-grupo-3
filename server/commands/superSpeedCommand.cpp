#include "superSpeedCommand.h"

SuperSpeedCommand::SuperSpeedCommand(ClientID clientId): Command(clientId) {}

void SuperSpeedCommand::execute(Game& game) {
    game.makePlayerGoSuperFast(getClientID());
}
