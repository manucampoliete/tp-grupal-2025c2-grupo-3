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
struct CheatSuperSpeedCommand {};


struct ModifyCarCommand {
    bool speedMod;
    bool healthMod;
    bool accelMod;
    bool massMod;
    
    ModifyCarCommand(bool speed, bool health, bool accel, bool mass) : speedMod(speed), healthMod(health), accelMod(accel), massMod(mass) {}
    ModifyCarCommand() : speedMod(false), healthMod(false), accelMod(false), massMod(false) {}
};


using ClientMessage = std::variant<
    MoveCommand,
    CheatInmortalityCommand,
    CheatInstaWinCommand,
    CheatInstaLoseCommand,
    CheatSuperSpeedCommand,
    ModifyCarCommand
>;

#endif  // CLIENT_MESSAGE_H