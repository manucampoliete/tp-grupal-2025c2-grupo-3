#ifndef GAME_H
#define GAME_H

#include <map>
#include <memory>
#include <string>
#include <utility>

#include <box2d/box2d.h>

#include "../common/queue/queue.h"
#include "../common/thread/thread.h"
#include "commands/command.h"

#include "car.h"
#include "player.h"
#include "responseQueuesMonitor.h"
#include "types.h"

class Game: public Thread {
private:
    b2World* world;
    int32 velocityIt;
    int32 positionIt;
    Queue<std::unique_ptr<Command>> clientCommandsQueue;
    ResponseQueuesMonitor responseQueuesMonitor;
    std::map<ClientID, Player> players;

    b2Body* createNewCarBody();

    void updatePlayerCars();

    void broadcast();

public:
    /**
     * Constructor
     */
    Game();

    /**
     * Main game logic: processes commands from the clientCommandsQ queue.
     * If Thread::shouldKeepRunning() becomes false, the game stops.
     * Any exception thrown by Queue::tryPop() or ResponseQueuesMonitor::broadcast()
     * is caught and logged in Thread::main() and causes the game to stop.
     * In all these cases, the game thread ends.
     */
    void run() override;

    /**
     * Calls Thread::stop(), setting shouldKeepRunning() = false
     * and then:
     * - closes the clientCommandsQ queue
     * - calls ResponseQueuesMonitor::closeAll()
     */
    void stop() override;

    /**
     * Returns a reference to the clientCommandsQ queue.
     */
    Queue<std::unique_ptr<Command>>& getClientCommandsQueue();

    /**
     * Returns a reference to the responseQueues monitor.
     */
    Queue<std::shared_ptr<Snapshot>>& getResponsesQueue(ClientID clientId);

    /**
     * Adds a new player to the game with the given parameters.
     */
    void addPlayer(ClientID clientId, const std::string& username, uint8_t carId);

    void movePlayer(ClientID clientId, ActiveDirections activeDirections);

    /**
     * Destructor
     * Nothing special to do
     */
    ~Game() override;
};

#endif  // GAME_H
