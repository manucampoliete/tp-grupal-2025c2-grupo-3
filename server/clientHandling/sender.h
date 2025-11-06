#ifndef SENDER_H
#define SENDER_H

#include <memory>

#include "../../common/queue/queue.h"
#include "../../common/thread/thread.h"
#include "../protocol/serverGameSendProtocol.h"

/**
 * Sender class: sends Snapshot responses to the client.
 */
class Sender: public Thread {
private:
    ServerGameSendProtocol protocol;
    Queue<std::shared_ptr<Snapshot>>& responsesQueue;

public:
    /**
     * Constructor: initializes the Sender with the given parameters.
     */
    explicit Sender(Socket& skt, Queue<std::shared_ptr<Snapshot>>& responsesQueue);

    /**
     * Main sender logic: pops Snapshot responses from its responsesQueue and sends them to the
     * client. If Thread::shouldKeepRunning() becomes false, the sender stops. Any exception thrown
     * by ServerGameSendProtocol::sendSnapshot() or Queue::pop() is caught, logged, and causes the
     * sender to stop. In all these cases, the sender thread ends.
     */
    void run() override;

    /**
     * Destructor
     * Nothing special to do
     */
    ~Sender() override = default;
};

#endif  // SENDER_H
