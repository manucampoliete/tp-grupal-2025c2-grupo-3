#include "improvementsCommand.h"

ImprovementsCommand::ImprovementsCommand(ClientID clientId, bool improveVelocity, bool improveHealth, bool improveAcceleration, bool improveMass)
    : Command(clientId), improveVelocity(improveVelocity), improveHealth(improveHealth), improveAcceleration(improveAcceleration), improveMass(improveMass) {}

void ImprovementsCommand::execute(Game& game) {
    game.improveCarProperties(getClientID(), improveVelocity, improveHealth, improveAcceleration, improveMass);
}
