#include "instaLoseCommand.h"

InstaLoseCommand::InstaLoseCommand(ClientID clientId): Command(clientId) {}

void InstaLoseCommand::execute(Game& game) { game.makeInstaLose(getClientID()); }
