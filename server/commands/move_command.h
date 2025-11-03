#ifndef MOVE_COMMAND_H
#define MOVE_COMMAND_H

#include "command.h"
#include "../common/utils/active_directions.h"

#include <cstdint>

/**
 * Command to move a player in specified directions.
 */
class MoveCommand : public Command {
    uint16_t client_id;
    ActiveDirections active_directions;
    
public:
    MoveCommand(uint16_t client_id, ActiveDirections directions) 
        : client_id(client_id), active_directions(directions) {}

    void execute(Game* game) override {
        // game->process_request();
    }
};

#endif // MOVE_COMMAND_H

