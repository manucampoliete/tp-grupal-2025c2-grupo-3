#ifndef GAME_H
#define GAME_H

#include <map>
#include <memory>
#include <string>
#include <utility>

#include <box2d/box2d.h>

#include "../../common/queue/queue.h"
#include "../../common/thread/thread.h"
#include "../commands/command.h"

#include "cars/car.h"
#include "player.h"
#include "../synchronized/responseQueuesMonitor.h"
#include "../../common/types/types.h"

class Command;

class Game: public Thread {
private:
    b2World* world;
    int32 velocityIt;
    int32 positionIt;
    Queue<std::unique_ptr<Command>> clientCommandsQueue;
    ResponseQueuesMonitor responseQueuesMonitor;
    std::map<ClientID, Player> players;

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
     * Destructor
     * Nothing special to do
     */
    ~Game() override;
};

#endif  // GAME_H
