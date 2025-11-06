#ifndef MOVE_COMMAND_H
#define MOVE_COMMAND_H

#include "../common/utils/activeDirections.h"

#include "command.h"

/**
 * Command to move a player in specified directions.
 */
class MoveCommand: public Command {
private:
    ActiveDirections activeDirections;

public:
    MoveCommand(ClientID clientId, ActiveDirections activeDirections);

    void execute(Game& game) override;
};

#endif  // MOVE_COMMAND_H
