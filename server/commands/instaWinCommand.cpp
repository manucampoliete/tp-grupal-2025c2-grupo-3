#include "instaWinCommand.h"

InstaWinCommand::InstaWinCommand(ClientID clientId): Command(clientId) {}

void InstaWinCommand::execute(Game& game) { game.makeInstaWin(getClientID()); }
