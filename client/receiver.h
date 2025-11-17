#ifndef RECEIVER_H
#define RECEIVER_H

#include "../common/messages/snapshot.h"
#include "../common/protocol/protocolConstants.h"
#include "../common/queue/queue.h"
#include "../common/thread/thread.h"
#include "../common/types/types.h"
#include "../common/utils/activeDirections.h"
#include "../protocol/clientProtocol.h"


class GameLoop;

/**
 * Receiver: thread that receives messages from the server
 * - Reads messages from the server using ClientProtocol
 * - Pushes snapshots to the queue
 * - Notifies events to the GameLoop
 */
class Receiver: public Thread {
private:
    ClientProtocol& protocol;
    Queue<Snapshot>& serverSnapshotsQueue;
    GameLoop& gameLoop;
    uint8_t lastCountdownNumber;

public:
    /**
     * Constructor: initializes the Receiver with the given parameters.
     * serverSnapshotsQueue: received snapshots are pushed to this queue
     * gameLoop: reference to the GameLoop to notify events
     */
    Receiver(ClientProtocol& protocol, Queue<Snapshot>& serverSnapshotsQueue, GameLoop& gameLoop);

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
