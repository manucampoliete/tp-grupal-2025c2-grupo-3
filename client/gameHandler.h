#ifndef GAME_HANDLER_H
#define GAME_HANDLER_H

#include <atomic>
#include <memory>

#include "../common/messages/game_data.h"
#include "../common/queue/queue.h"
#include "../common/utils/activeDirections.h"
#include "../protocol/clientProtocol.h"

#include "gameLoop.h"
#include "receiver.h"
#include "sender.h"
#include "world.h"
#include "serverMessage.h"


/**
 * GameHandler: coordinates the game logic on the client
 * - Handles the World (game state)
 * - Handles Sender, Receiver, and GameLoop
 * - Process server events
 */
class GameHandler {
private:
    ClientProtocol& protocol;
    ClientID clientId;

    World world;

    Queue<ActiveDirections> clientRequestsQueue;
    Queue<ServerMessage> serverMessagesQueue;

    Sender sender;
    Receiver receiver;
    GameLoop gameLoop;

    std::atomic<bool> running;

public:
    GameHandler(ClientProtocol& protocol, ClientID clientId);

    // Runs the entire game: starts the 3 threads
    void run();

    // Stops the game and the threads
    void stop();

    ~GameHandler();
};

#endif  // GAME_HANDLER_H
