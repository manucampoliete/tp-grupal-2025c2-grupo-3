#ifndef SENDER_H
#define SENDER_H

#include "../common/queue/queue.h"
#include "../common/thread/thread.h"
#include "../common/commands/move_request.h"

#include "../common/protocol/dummy_client_protocol.h"

class Sender: public Thread {
private:
    DummyClientProtocol& protocol;
    Queue<MoveRequest>& client_requests_q;

public:
    /**
     * Constructor: initializes the Sender with the given parameters.
     */
    Sender(DummyClientProtocol& protocol, Queue<MoveRequest>& client_requests_q);

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
