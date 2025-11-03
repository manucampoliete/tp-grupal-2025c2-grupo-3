
#ifndef RESPONSE_QUEUES_MONITOR_H
#define RESPONSE_QUEUES_MONITOR_H

#include <map>
#include <mutex>

#include "../common/queue/queue.h"
#include "../common/utils/vector_2d.h"
#include "../common/messages/snapshot.h"

#include "types.h"

class ResponseQueuesMonitor {
private:
    std::map<ClientID, Queue<Snapshot>> response_queues;
    std::mutex mtx;

public:
    /**
     * Constructor: initializes an empty ResponseQueuesMonitor.
     */
    ResponseQueuesMonitor();

    /**
     * Adds a new response queue for the given client_id.
     * If a queue for the given client_id already exists, returns a reference to it.
     * Otherwise, creates a new queue, adds it to the map, and returns a reference to it.
     */
    Queue<Snapshot>& add_queue(ClientID client_id);

    /**
     * Removes the response queue for the given client_id.
     * If a queue for the given client_id exists, removes it from the map and returns 1.
     * Otherwise, returns 0.
     * Note: this does not close the queue. The Sender associated with the queue should have been
     * stopped before calling this method.
     */
    std::size_t remove_queue(ClientID client_id);

    /**
     * Broadcasts the given response to all response queues.
     * Returns true if all try_push operations succeeded, false otherwise.
     * Note: because try_push is used, this method does not block the gameloop.
     */
    bool broadcast(const Snapshot& resp);

    /**
     * Closes all response queues.
     * This unblocks any Sender that is blocked on Queue::pop().
     * After calling this method, no more responses can be pushed to any queue.
     * This method is typically called when the server is shutting down.
     */
    void close_all();

    /**
     * Destructor
     * Nothing special to do
     */
    ~ResponseQueuesMonitor();
};

#endif  // RESPONSE_QUEUES_MONITOR_H
