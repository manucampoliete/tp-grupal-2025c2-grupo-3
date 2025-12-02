#ifndef SENDER_H
#define SENDER_H

#include "../../common/queue/queue.h"
#include "../../common/thread/thread.h"
#include "../utils/clientMessage.h"
#include "../protocol/clientGameSendProtocol.h"


/**
 * Sender: thread that sends client commands to the server
 * - Pops commands from the commandQueue queue
 * - Serializes and sends them using ClientGameSendProtocol
 */
class Sender: public Thread {
private:
    ClientGameSendProtocol protocol;
    Queue<ClientMessage>& commandQueue;

public:
    /**
     * Constructor: initializes the Sender with the given parameters.
     * commandQueue: queue of commands to be sent to the server
     */
    Sender(Socket& skt, Queue<ClientMessage>& commandQueue);

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
