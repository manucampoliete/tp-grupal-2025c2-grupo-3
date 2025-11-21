#ifndef RECEIVER_H
#define RECEIVER_H

#include "../../common/messages/snapshot.h"
#include "../../common/protocol/protocolConstants.h"
#include "../../common/queue/queue.h"
#include "../../common/thread/thread.h"
#include "../../common/types/types.h"
#include "../protocol/clientGameProtocol.h"
#include "../../common/messages/serverMessage.h"

/**
 * Receiver: thread that receives messages from the server
 * - Reads messages from the server using ClientGameProtocol
 * - Pushes snapshots to the queue
 * - Notifies events to the GameLoop
 */
class Receiver: public Thread {
private:
    ClientGameProtocol& protocol;
    Queue<ServerMessage>& serverMessagesQueue;

public:
    /**
     * Constructor: initializes the Receiver with the given parameters.
     * serverMessagesQueue: received messages are pushed to this queue
     */
    Receiver(ClientGameProtocol& protocol, Queue<ServerMessage>& serverMessagesQueue);

    /**
     * TODO: Add proper documentation
     */
    void run() override;

    /**
     * Destructor
     * Nothing special to do
     */
    ~Receiver() override = default;
};

#endif  // RECEIVER_H
