#ifndef RECEIVER_H
#define RECEIVER_H

#include "../common/queue/queue.h"
#include "../common/thread/thread.h"
#include "../common/commands/move_request.h"
#include "../common/protocol/dummy_client_protocol.h"
#include "../server/types.h"
#include <vector>


class Client;

class Receiver: public Thread {
private:
    DummyClientProtocol& protocol;
    Queue<std::vector<std::pair<ClientID, Vector2D>>>& server_responses_q; 
    Client& client; // referencia para avisarle de las actualizaciones

public:
    /**
     * Constructor: initializes the Receiver with the given parameters.
     */
    Receiver(DummyClientProtocol& protocol, Queue<std::vector<std::pair<ClientID, Vector2D>>>& server_responses_q, Client& client);

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
