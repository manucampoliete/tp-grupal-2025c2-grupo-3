#ifndef SENDER_H
#define SENDER_H

#include "../../common/queue/queue.h"
#include "../../common/thread/thread.h"
#include "../../common/messages/clientMessage.h"
#include "../protocol/clientGameProtocol.h"


/**
 * Sender: thread that sends client commands to the server
 * - Pops commands from the commandQueue queue
 * - Serializes and sends them using ClientGameProtocol
 */
class Sender: public Thread {
private:
    ClientGameProtocol& protocol;
    Queue<ClientMessage>& commandQueue;

public:
    /**
     * Constructor: initializes the Sender with the given parameters.
     * commandQueue: queue of commands to be sent to the server
     */
    Sender(ClientGameProtocol& protocol, Queue<ClientMessage>& commandQueue);

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
