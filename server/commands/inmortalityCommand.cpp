#include "inmortalityCommand.h"

InmortalityCommand::InmortalityCommand(ClientID clientId)
    : Command(clientId) {}

void InmortalityCommand::execute(Game& game) {
    // game.makeInmortal(getClientID());
}
