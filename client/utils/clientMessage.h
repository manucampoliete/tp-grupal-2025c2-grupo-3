#ifndef CLIENT_MESSAGE_H
#define CLIENT_MESSAGE_H

#include <variant>

#include "../../common/utils/activeDirections.h"


struct MoveCommand {
    ActiveDirections directions;
    
    explicit MoveCommand(const ActiveDirections& dirs) : directions(dirs) {}
    MoveCommand() : directions() {}
};


struct CheatInmortalityCommand {};
struct CheatInstaWinCommand {};
struct CheatInstaLoseCommand {};


struct ModifyCarCommand {
    bool speedMod;
    bool healthMod;
    
    ModifyCarCommand(bool speed, bool health) : speedMod(speed), healthMod(health) {}
    ModifyCarCommand() : speedMod(false), healthMod(false) {}
};


using ClientMessage = std::variant<
    MoveCommand,
    CheatInmortalityCommand,
    CheatInstaWinCommand,
    CheatInstaLoseCommand,
    ModifyCarCommand
>;

#endif  // CLIENT_MESSAGE_H