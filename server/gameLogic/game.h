#ifndef GAME_H
#define GAME_H

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <chrono>

#include <box2d/box2d.h>

#include "../../common/queue/queue.h"
#include "../../common/thread/thread.h"
#include "../../common/types/types.h"
#include "../../common/utils/gameState.h"
#include "../../common/utils/mapConstants.h"

#include "../commands/command.h"
#include "../synchronized/responseQueuesMonitor.h"
#include "../configLoader.h"

#include "car.h"
#include "player.h"

#include "collisions/pathLoader.h"
#include "collisions/pathGenerator.h"
#include "collisions/graphLoader.h"

class Command;

class Game: public Thread {
private:
    const Config& config;
    std::unique_ptr<b2World> world;
    int32 velocityIt;
    int32 positionIt;

    Queue<std::unique_ptr<Command>> clientCommandsQueue;
    ResponseQueuesMonitor responseQueuesMonitor;

    std::map<ClientID, Player> players;

    //carreras
    uint8_t races;
    uint8_t currentRaceCount;
    std::vector<std::string> raceFiles;

    //tiempos
    std::chrono::seconds countdownDuration;
    std::chrono::minutes raceDuration;
    std::chrono::seconds statsDuration;
    std::chrono::seconds upgradesDuration;
    // std::chrono::duration<float> elapsed;

    GameState currentState;
    std::chrono::high_resolution_clock::time_point gameStateStartTime;

    std::atomic<bool> started;

    std::vector<b2Body*> pathBodies;
    std::vector<b2Body*> lowCollisionLayerBodies;
    std::vector<b2Body*> highCollisionLayerBodies;
    std::vector<b2Body*> layerSwitchBodies;
    std::vector<b2Body*> NPCGraphBodies;

    /**
     * Creates and returns a new b2Body for a car.
     */
    b2Body* createNewCarBody();

    void clearMapBodies();

    /**
     * Updates the physics of all player cars.
     */
    void updatePlayerCars();

    /**
     * Builds and broadcasts a Snapshot to all players.
     */
    void broadcast();

    /**
     * Broadcasts a start signal to all players.
     */
    void broadcastStartSignal();

    void broadcastCountdown();
    void broadcastRacing();
    void broadcastShowingStats();
    void broadcastModifyingCar();
    // void broadcastEliminated(); // seguro no se va a usar
    void broadcastGameEnd();

    void setGameState(GameState new_state);
    void setCountdownState();
    void setRacingState();
    void setShowingStatsState();
    void setModifyingCarState();
    // void setEliminatedState();
    void setGameEndState();

    void updateGameState();

    std::chrono::seconds getRemainingGameStateTime();

    void handleGameState();

    void handleCountdownState();
    void handleRacingState();
    void handleShowingStatsState();
    void handleModifyingCarState();
    // void handleEliminatedState();
    void handleGameEndState();

    bool allPlayersFinished();

    /**
     * Calls Thread::stop(), setting shouldKeepRunning() = false
     * and then:
     * - closes the clientCommandsQueue
     * - calls ResponseQueuesMonitor::closeAll()
     */
    void stop() override;
    
    void join() override;

public:
    /**
     * Constructor
     */
    Game(const Config& config);

    /**
     * Main game logic: processes commands from the clientCommandsQueue.
     * If Thread::shouldKeepRunning() becomes false, the game stops.
     * Any exception thrown by Queue::tryPop() or ResponseQueuesMonitor::broadcast()
     * is caught and logged in Thread::main() and causes the game to stop.
     * In all these cases, the game thread ends.
     */
    void run() override;
    
    /**
     * Returns true if the game thread is still running, false otherwise.
     */
    bool isAlive() const override;

    /**
     * Returns true if the game thread has ended, false otherwise.
     */
    bool isDead();

    /**
     * Returns a reference to the clientCommandsQueue.
     */
    Queue<std::unique_ptr<Command>>& getClientCommandsQueue();

    /**
     * Returns a reference to the response queue for the given clientId.
     */
    Queue<std::shared_ptr<Snapshot>>& getResponsesQueue(ClientID clientId);

    /**
     * Adds a new player to the game with the given parameters. 
     * Returns true if the player was successfully added, false otherwise.
     */
    bool addPlayer(ClientID clientId, const std::string& username, CarID carId);

    /**
     * Moves the player with the given clientId according to the given activeDirections.
     */
    void movePlayer(ClientID clientId, ActiveDirections activeDirections);

    void handleCollision(Player* player, float impact);
    void handleCheckpointContact(Player* player, PathElement& chk);

    /**
     * Cheats!
     */
    void makeInmortal(ClientID clientId);
    void makeInstaWin(ClientID clientId);
    void makeInstaLose(ClientID clientId);
    void makePlayerGoSuperFast(ClientID clientId);

    /**
     * Improves the car properties of the player with the given clientId.
     */
    void improveCarProperties(ClientID clientId, bool improveVelocity, bool improveHealth, bool improveAcceleration, bool improveMass);

    /**
     * Disconnects the player with the given clientId from the game.
     */
    void disconnectPlayer(ClientID clientId);

    /**
     * Destructor: cleans up the game.
     */
    ~Game() override;
};

#endif  // GAME_H
