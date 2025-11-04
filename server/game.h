#ifndef GAME_H
#define GAME_H

#include <map>

#include "../common/queue/queue.h"
#include "../common/thread/thread.h"
#include "../common/utils/vector_2d.h"

#include "../common/commands/move_request_with_id.h"
#include "response_queues_monitor.h"
#include "types.h"

class Game: public Thread {
private:
    Queue<MoveRequestWithID> client_commands_q;
    ResponseQueuesMonitor response_queues;
    std::map<ClientID, Vector2D> positions;

    /**
     * Processes a single MoveRequestWithID: updates the position of the client
     */
    void process_request(const MoveRequestWithID& req, float delta_time);

public:
    /**
     * Constructor
     */
    Game();
    
    /**
     * Main game logic: processes move requests from the client_commands_q queue.
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
    Queue<MoveRequestWithID>& get_client_commands_queue();

    /**
     * Returns a reference to the response_queues monitor.
     */
    ResponseQueuesMonitor& get_response_queues_monitor();

    /**
     * Destructor
     * Nothing special to do
     */
    ~Game() override;
};

#endif  // GAME_H
