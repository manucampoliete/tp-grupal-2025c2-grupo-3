#ifndef DISCONNECT_PLAYER_COMMAND_H
#define DISCONNECT_PLAYER_COMMAND_H

#include "command.h"

class DisconnectPlayerCommand: public Command {
public:
    DisconnectPlayerCommand(ClientID clientId);

    void execute(Game& game) override;
};

#endif  // DISCONNECT_PLAYER_COMMAND_H
