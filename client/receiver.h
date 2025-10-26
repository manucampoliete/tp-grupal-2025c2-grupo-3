#ifndef RECEIVER_H
#define RECEIVER_H

#include "../common/queue/queue.h"
#include "../common/thread/thread.h"
#include "../common/commands/move_request.h"
#include "../common/protocol/dummy_client_protocol.h"

class Receiver: public Thread {
private:
    DummyClientProtocol& protocol;
    Queue<std::vector<std::pair<ClientID, Vector2D>>>& server_responses_q;

public:
    /**
     * Constructor: initializes the Receiver with the given parameters.
     */
    Receiver(DummyClientProtocol& protocol, Queue<std::vector<std::pair<ClientID, Vector2D>>>& server_responses_q);

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
