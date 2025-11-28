#include "disconnectPlayerCommand.h"

DisconnectPlayerCommand::DisconnectPlayerCommand(ClientID clientId): Command(clientId) {}

void DisconnectPlayerCommand::execute(Game& game) {
    game.disconnectPlayer(getClientID());
}
