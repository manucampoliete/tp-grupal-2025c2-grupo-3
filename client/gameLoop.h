#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include <memory>

#include "../common/messages/gameData.h"
#include "../common/queue/queue.h"
#include "../common/thread/thread.h"
#include "../common/utils/activeDirections.h"
#include "protocol/clientGameProtocol.h"

#include "serverMessage.h"
#include "world.h"


class Game;

/**
 * GameLoop: thread that runs the main game loop with SDL
 * - Processes snapshots from Receiver's queue
 * - Handles user input
 * - Pushes commands to the Sender's queue
 */
class GameLoop: public Thread {
private:
    Queue<ServerMessage>& serverMessagesQueue;
    Queue<ActiveDirections>& clientRequestsQueue;
    ClientGameProtocol& protocol;

    World& world;
    ClientID clientId;

    std::unique_ptr<Game> game;
    uint8_t lastCountdownNumber;
    
    /**
     * Methods used by other threads to notify events
     */
    void onCountdown(uint8_t number);
    void onRaceStart();
    void onCheckpointCrossed(uint8_t checkpointId);
    void onCollision(const CollisionData& collision);
    void onPlayerDied(ClientID deadPlayerId);
    void onRaceEnd(const RaceResults& results);
    void onModificationPhase(const CarProperties& props);
    void onGameEnd(const FinalResults& results);

public:
    /**
     * Constructor
     */
    GameLoop(Queue<ServerMessage>& serverMessagesQueue, 
             Queue<ActiveDirections>& clientRequestsQueue,
             ClientGameProtocol& protocol, World& world, ClientID clientId);

    /**
     * Main loop that runs in the thread
     */
    void run() override;

    /**
     * Methods to send commands (called by Game)
     */
    void sendMovement(bool up, bool down, bool left, bool right);
    void sendModifications(bool speed, bool health);
    void sendCheatInmortality();
    void sendCheatInstaWin();
    void sendCheatInstaLose();

    ~GameLoop() override = default;
};

#endif  // GAME_LOOP_H
