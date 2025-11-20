#include "improvementsCommand.h"

ImprovementsCommand::ImprovementsCommand(ClientID clientId, bool improveVelocity, bool improveHealth)
    : Command(clientId), improveVelocity(improveVelocity), improveHealth(improveHealth) {}

void ImprovementsCommand::execute(Game& game) {
    game.improveCarProperties(getClientID(), improveVelocity, improveHealth);
}
