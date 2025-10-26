#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <utility>
#include <vector>

#include "../common/commands/move_request.h"
#include "../common/protocol/dummy_client_protocol.h"
#include "../common/queue/queue.h"
#include "../common/utils/vector_2d.h"
#include "../server/types.h"

#include "receiver.h"
#include "sender.h"

class Client {
private:
    Socket socket;
    DummyClientProtocol protocol;
    Queue<MoveRequest> client_requests_q;
    Queue<std::vector<std::pair<ClientID, Vector2D>>> server_responses_q;
    Sender sender;
    Receiver receiver;

    /**
     * Disable copy semantics (not needed and error-prone)
     */
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

public:
    /**
     * Constructor: takes the server hostname and service name (to connect to)
     */
    Client(const std::string& server_hostname, const std::string& server_servname);

    /**
     * Enable move semantics (default implementations are fine)
     */
    Client(Client&&) = default;
    Client& operator=(Client&&) = default;

    /**
     * Runs the client: starts the sender and receiver threads,
     * handles user input and displays server responses.
     * Returns EXIT_SUCCESS if everything went fine, or EXIT_FAILURE otherwise.
     */
    int run();

    /**
     * Destructor
     * Nothing special to do
     */
    ~Client();
};

#endif  // CLIENT_H
