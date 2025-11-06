#ifndef GAME_RESOLVER_H
#define GAME_RESOLVER_H

#include "../common/queue/queue.h"
#include "../common/types/types.h"
#include "../common/utils/activeDirections.h"
#include "../commands/moveCommand.h"

class GameResolver {
private:
    ClientID client_id;
    Queue<std::unique_ptr<Command>>& clientCommandsQueue;

public:
    /**
     * Constructor
     */
    GameResolver(ClientID clientId, Queue<std::unique_ptr<Command>>& clientCommandsQueue);

    /**
     * Handles a MOVE request from the client.
     */
    void handleMove(const ActiveDirections& directions);
};

#endif // GAME_RESOLVER_H
