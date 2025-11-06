#ifndef COMMAND_H
#define COMMAND_H

#include "../../common/types/types.h"
#include "../gameLogic/game.h"

/**
 * Interface for commands that can be executed by the GameController.
 */
class Command {
private:
    ClientID client_id;

protected:
    ClientID getClientID() const;

public:
    Command(ClientID id);
    virtual void execute(Game& game) = 0;
    virtual ~Command() = default;
};

#endif  // COMMAND_H
