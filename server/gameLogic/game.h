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
#include "../commands/command.h"
#include "../synchronized/responseQueuesMonitor.h"
#include "car.h"
#include "player.h"

#include "../../common/utils/gameState.h"

class Command;

class Game: public Thread {
private:
    b2World* world;
    int32 velocityIt;
    int32 positionIt;
    Queue<std::unique_ptr<Command>> clientCommandsQueue;
    ResponseQueuesMonitor responseQueuesMonitor;
    std::map<ClientID, Player> players;

    //tiempos
    std::chrono::seconds countdownDuration;
    std::chrono::minutes raceDuration;
    std::chrono::seconds statsDuration;
    std::chrono::seconds upgradesDuration;
    // std::chrono::duration<float> elapsed;

    game_state current_state;
    std::chrono::high_resolution_clock::time_point gameStateStartTime;

    /**
     * Creates and returns a new b2Body for a car.
     */
    b2Body* createNewCarBody();

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
    void broadcast_start_signal();

    void setGameState(game_state new_state);

    void updateGameState();

    std::chrono::seconds getRemainingGameStateTime();

    void handleGameState(float deltaTime);

    void handleCountdownState();
    void handleRacingState(float deltaTime);
    void handleShowingStatsState();
    void handleModifyingCarState();
    // void handleEliminatedState();
    void handleGameEndState();

public:
    /**
     * Constructor
     */
    Game();

    /**
     * Main game logic: processes commands from the clientCommandsQueue.
     * If Thread::shouldKeepRunning() becomes false, the game stops.
     * Any exception thrown by Queue::tryPop() or ResponseQueuesMonitor::broadcast()
     * is caught and logged in Thread::main() and causes the game to stop.
     * In all these cases, the game thread ends.
     */
    void run() override;

    /**
     * Calls Thread::stop(), setting shouldKeepRunning() = false
     * and then:
     * - closes the clientCommandsQueue
     * - calls ResponseQueuesMonitor::closeAll()
     */
    void stop() override;

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
     */
    void addPlayer(ClientID clientId, const std::string& username, CarID carId);

    /**
     * Moves the player with the given clientId according to the given activeDirections.
     */
    void movePlayer(ClientID clientId, ActiveDirections activeDirections);

    /**
     * Cheats!
     */
    void makeInmortal(ClientID clientId);
    void makeInstaWin(ClientID clientId);
    void makeInstaLose(ClientID clientId);

    /**
     * Destructor
     * Nothing special to do
     */
    ~Game() override;
};

#endif  // GAME_H
