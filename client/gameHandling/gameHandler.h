#ifndef GAME_HANDLER_H
#define GAME_HANDLER_H

#include <atomic>
#include <memory>

#include "../../common/queue/queue.h"
#include "../utils/gameData.h"
#include "../protocol/clientGameProtocol.h"

#include "../threads/gameLoop.h"
#include "../threads/receiver.h"
#include "../threads/sender.h"
#include "world.h"
#include "../utils/serverMessage.h"
#include "../utils/clientMessage.h"


/**
 * GameHandler: coordinates the game logic on the client
 * - Handles the World (game state)
 * - Handles Sender, Receiver, and GameLoop
 * - Process server events
 */
class GameHandler {
private:
    ClientGameProtocol protocol;
    ClientID clientId;

    World world;

    Queue<ClientMessage> clientCommandQueue;
    Queue<ServerMessage> serverMessagesQueue;

    Sender sender;
    Receiver receiver;
    GameLoop gameLoop;

    std::atomic<bool> running;

public:
    GameHandler(Socket& skt, ClientID clientId);

    // Runs the entire game: starts the 3 threads
    void run();

    // Stops the game and the threads
    void stop();

    ~GameHandler();
};

#endif  // GAME_HANDLER_H
