#ifndef RECEIVER_H
#define RECEIVER_H

#include "../common/queue/queue.h"
#include "../common/thread/thread.h"

#include "../common/commands/move_request_with_id.h"
#include "../common/protocol/dummy_server_protocol.h"
#include "types.h"

class Receiver: public Thread {
private:
    DummyServerProtocol& protocol;
    Queue<MoveRequestWithID>& client_commands_q;
    const ClientID client_id;

public:
    /**
     * Constructor: initializes the Receiver with the given parameters.
     */
    Receiver(DummyServerProtocol& protocol, Queue<MoveRequestWithID>& client_commands_q, ClientID client_id);

     /**
     * Main receiver logic: receives move requests from the client and pushes them to the
     * client_commands_q queue, wrapping them in a MoveRequestWithID with the client_id. If
     * Thread::should_keep_running() becomes false, the receiver stops. Any exception thrown by
     * DummyServerProtocol::recv_move_request() (when client disconnects) or Queue::push() is caught, 
     * logged, and causes the receiver to stop. In all these cases, the receiver thread ends.
     */
    void run() override;

    /**
     * Destructor
     * Nothing special to do
     */
    ~Receiver() override = default;
};

#endif  // RECEIVER_H
