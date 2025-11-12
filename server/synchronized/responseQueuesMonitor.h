#ifndef RESPONSE_QUEUES_MONITOR_H
#define RESPONSE_QUEUES_MONITOR_H

#include <map>
#include <memory>
#include <mutex>

#include "../../common/messages/snapshot.h"
#include "../../common/queue/queue.h"
#include "../../common/types/types.h"

class ResponseQueuesMonitor {
private:
    std::map<ClientID, Queue<std::shared_ptr<Snapshot>>> responseQueues;
    std::mutex mtx;

public:
    /**
     * Constructor: initializes an empty ResponseQueuesMonitor.
     */
    ResponseQueuesMonitor();

    /**
     * Adds a new response queue for the given clientId.
     * If a queue for the given clientId already exists, returns a reference to it.
     * Otherwise, creates a new queue, adds it to the map, and returns a reference to it.
     */
    Queue<std::shared_ptr<Snapshot>>& addQueue(ClientID clientId);

    /**
     * Returns a reference to the response queue for the given clientId.
     * If no queue for the given clientId exists, throws std::out_of_range.
     */
    Queue<std::shared_ptr<Snapshot>>& getQueue(ClientID clientId);

    /**
     * Removes the response queue for the given clientId.
     * If a queue for the given clientId exists, removes it from the map and returns 1.
     * Otherwise, returns 0.
     * Note: the Sender associated with the queue should have been
     * stopped before calling this method.
     */
    std::size_t removeQueue(ClientID clientId);

    /**
     * Broadcasts the given snapshot to all response queues.
     * Returns true if all tryPush operations succeeded, false otherwise.
     * Note: because tryPush is used, this method does not block the gameloop.
     */
    bool broadcast(std::shared_ptr<Snapshot> snapshot);

    /**
     * Closes all response queues.
     * This unblocks any Sender that is blocked on Queue::pop().
     * After calling this method, no more responses can be pushed to any queue.
     * This method is typically called when the server is shutting down.
     */
    void closeAll();

    /**
     * Destructor
     * Nothing special to do
     */
    ~ResponseQueuesMonitor();
};

#endif  // RESPONSE_QUEUES_MONITOR_H
