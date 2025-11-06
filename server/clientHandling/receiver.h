#ifndef RECEIVER_H
#define RECEIVER_H

#include "../protocol/serverGameRecvProtocol.h"
#include "../common/queue/queue.h"
#include "../common/types/types.h"
#include "../commands/command.h"

#include <memory>

/**
 * Receiver class: receives Command requests from the client.
 */
class Receiver : public Thread {
private:
    ServerGameRecvProtocol protocol;
    GameResolver gameResolver;

public:
    /**
     * Constructor: initializes the Receiver with the given parameters.
     */
    Receiver(Socket& skt, ClientID clientId, Queue<std::unique_ptr<Command>>& clientCommandsQueue);

    /**
     * This is a fake start(). No std::thread is created here. This method just
     * calls run() directly.
     */
    void start() override;
    
    /**
     * TODO: add proper documentation
     */
    void run();

    /**
     * Destructor
     * Nothing special to do
     */
    ~Receiver() = default;
};

#endif  // RECEIVER_H
