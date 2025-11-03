#ifndef GAME_H
#define GAME_H

#include <map>

#include "../common/queue/queue.h"
#include "../common/thread/thread.h"
#include "../common/utils/vector_2d.h"

#include "response_queues_monitor.h"
#include "types.h"
#include "commands/command.h"

class Game: public Thread {
private:
    Queue<std::unique_ptr<Command>> client_commands_q;
    ResponseQueuesMonitor response_queues;

public:
    /**
     * Constructor
     */
    Game();
    
    /**
     * Main game logic: processes commands from the client_commands_q queue.
     * If Thread::should_keep_running() becomes false, the game stops.
     * Any exception thrown by Queue::try_pop() or ResponseQueuesMonitor::broadcast()
     * is caught and logged in Thread::main() and causes the game to stop.
     * In all these cases, the game thread ends.
     */
    void run() override;

    /**
     * Calls Thread::stop(), setting should_keep_running() = false
     * and then:
     * - closes the client_commands_q queue
     * - calls ResponseQueuesMonitor::close_all()
     */
    void stop() override;

    /**
     * Returns a reference to the client_commands_q queue.
     */
    Queue<std::unique_ptr<Command>>& get_client_commands_queue();

    /**
     * Returns a reference to the response_queues monitor.
     */
    Queue<Snapshot>& get_responses_queue(ClientID client_id);

    /**
     * Returns the commands and responses queues for the given client_id.
     */
    std::pair<Queue<std::unique_ptr<Command>>&, Queue<Snapshot>&> get_queues(ClientID client_id);

    /**
     * Adds a new player to the game with the given parameters.
     */
    void add_player(ClientID client_id, const std::string& username, uint8_t car_id);

    /**
     * Destructor
     * Nothing special to do
     */
    ~Game() override;
};

#endif  // GAME_H
