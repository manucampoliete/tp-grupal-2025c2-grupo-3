#ifndef COMMAND_H
#define COMMAND_H

/**
 * Interface for commands that can be executed by the GameController.
 */
class Command {
public:
    virtual ~Command() = default;
    virtual void execute(/*GameController& controller*/) = 0;
};

#endif // COMMAND_H
