#ifndef GAME_RESOLVER_H
#define GAME_RESOLVER_H

#include <memory>

#include "../../common/queue/queue.h"
#include "../../common/types/types.h"
#include "../../common/utils/activeDirections.h"
#include "../commands/command.h"

class GameResolver {
private:
    ClientID clientId;
    Queue<std::unique_ptr<Command>>& clientCommandsQueue;

public:
    /**
     * Constructor
     */
    GameResolver(ClientID clientId, Queue<std::unique_ptr<Command>>& clientCommandsQueue);

    /**
     * Handles a MOVE request from the client.
     */
    void handleMove(const ActiveDirections& activeDirections);

    /**
     * Handles an INMORTALITY request from the client.
     */
    void handleInmortality();

    /**
     * Handles an INSTA_WIN request from the client.
     */
    void handleInstaWin();

    /**
     * Handles an INSTA_LOSE request from the client.
     */
    void handleInstaLose();

    /**
     * Handles a SUPER_SPEED request from the client.
     */
    void handleSuperSpeed();

    /**
     * Handles a MODIFY_CAR request from the client.
     */
    void handleModifyCar(bool improveVelocity, bool improveHealth, bool improveAcceleration, bool improveMass);
};

#endif  // GAME_RESOLVER_H
