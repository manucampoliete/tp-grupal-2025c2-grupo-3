#ifndef SENDER_H
#define SENDER_H

#include "../../common/queue/queue.h"
#include "../../common/thread/thread.h"
#include "../../common/utils/activeDirections.h"
#include "../protocol/clientGameProtocol.h"


/**
 * Sender: thread that sends client commands to the server
 * - Pops commands from the clientRequestsQueue queue
 * - Serializes and sends them using ClientGameProtocol
 */
class Sender: public Thread {
private:
    ClientGameProtocol& protocol;
    Queue<ActiveDirections>& clientRequestsQueue;

public:
    /**
     * Constructor: initializes the Sender with the given parameters.
     * clientRequestsQueue: queue of commands to be sent to the server
     */
    Sender(ClientGameProtocol& protocol, Queue<ActiveDirections>& clientRequestsQueue);

    /**
     * TODO: add proper documentation
     */
    void run() override;

    /**
     * Destructor
     * Nothing special to do
     */
    ~Sender() override = default;
};

#endif  // SENDER_H
