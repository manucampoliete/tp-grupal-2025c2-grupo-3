#ifndef INSTA_WIN_COMMAND_H
#define INSTA_WIN_COMMAND_H

#include "command.h"

class InstaWinCommand: public Command {
public:
    explicit InstaWinCommand(ClientID clientId);

    void execute(Game& game) override;
};

#endif  // INSTA_WIN_COMMAND_H
