#ifndef SENDER_H
#define SENDER_H

#include "../common/queue/queue.h"
#include "../common/thread/thread.h"

#include "../common/protocol/dummy_server_protocol.h"

class Sender: public Thread {
private:
    DummyServerProtocol& protocol;
    Queue<std::vector<std::pair<ClientID, Vector2D>>>& responses_q;

public:
    /**
     * Constructor: initializes the Sender with the given parameters.
     */
    Sender(DummyServerProtocol& protocol, Queue<std::vector<std::pair<ClientID, Vector2D>>>& responses_q);

    /**
     * Main sender logic: pops positions responses from its responses_q and sends them to the client.
     * If Thread::should_keep_running() becomes false, the sender stops. Any exception thrown by
     * DummyServerProtocol::send_positions_response() or Queue::pop() is caught,
     * logged, and causes the sender to stop. In all these cases, the sender thread ends.
     */
    void run() override;

    /**
     * Destructor
     * Nothing special to do
     */
    ~Sender() override = default;
};

#endif  // SENDER_H