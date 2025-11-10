#ifndef INMORTALITY_COMMAND_H
#define INMORTALITY_COMMAND_H

#include "command.h"

class InmortalityCommand: public Command {
public:
    explicit InmortalityCommand(ClientID clientId);

    void execute(Game& game) override;
};

#endif  // INMORTALITY_COMMAND_H
