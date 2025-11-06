#ifndef COMMAND_H
#define COMMAND_H

#include "../../common/types/types.h"
#include "../gameLogic/game.h"

class Game;

/**
 * Interface for commands that can be executed by the Game.
 */
class Command {
private:
    ClientID clientId;

protected:
    ClientID getClientID() const;

public:
    explicit Command(ClientID clientId);
    virtual void execute(Game& game) = 0;
    virtual ~Command() = default;
};

#endif  // COMMAND_H
