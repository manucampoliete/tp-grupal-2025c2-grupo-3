#ifndef RECEIVER_H
#define RECEIVER_H

#include <atomic>
#include <memory>

#include "../../common/queue/queue.h"
#include "../../common/types/types.h"
#include "../commands/command.h"
#include "../protocol/serverGameRecvProtocol.h"

/**
 * Receiver class: receives Command requests from the client.
 */
class Receiver {
private:
    GameResolver gameResolver;
    ServerGameRecvProtocol protocol;
    std::atomic<bool> _keepRunning;  // Emulate a Thread-like behavior

public:
    /**
     * Constructor: initializes the Receiver with the given parameters.
     */
    Receiver(Socket& skt, Queue<std::unique_ptr<Command>>& clientCommandsQueue, ClientID clientId);

    /**
     * Stops the Receiver's run loop using an atomic flag.
     */
    void stop();

    /**
     * Run method: continuously consumes protocol messages from the socket
     * and processes them using the GameResolver until stopped or an error occurs.
     */
    void run();

    /**
     * Destructor
     * Nothing special to do
     */
    ~Receiver() = default;
};

#endif  // RECEIVER_H
