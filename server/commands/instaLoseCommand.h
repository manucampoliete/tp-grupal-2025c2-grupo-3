#ifndef INSTA_LOSE_COMMAND_H
#define INSTA_LOSE_COMMAND_H

#include "command.h"

class InstaLoseCommand: public Command {
public:
    explicit InstaLoseCommand(ClientID clientId);

    void execute(Game& game) override;
};

#endif  // INSTA_LOSE_COMMAND_H
