#ifndef IMPROVEMENTS_COMMAND_H
#define IMPROVEMENTS_COMMAND_H

#include "command.h"

class ImprovementsCommand: public Command {
private:
    bool improveVelocity;
    bool improveHealth;
    bool improveAcceleration;
    bool improveMass;
public:
    ImprovementsCommand(ClientID clientId, bool improveVelocity, bool improveHealth, bool improveAcceleration, bool improveMass);

    void execute(Game& game) override;
};

#endif  // IMPROVEMENTS_COMMAND_H
