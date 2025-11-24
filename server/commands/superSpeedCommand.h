#ifndef SUPER_SPEED_COMMAND_H
#define SUPER_SPEED_COMMAND_H

#include "command.h"

class SuperSpeedCommand: public Command {
public:
    explicit SuperSpeedCommand(ClientID clientId);

    void execute(Game& game) override;
};

#endif  // SUPER_SPEED_COMMAND_H
