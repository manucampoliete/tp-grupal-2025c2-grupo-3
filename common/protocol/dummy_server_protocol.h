#ifndef DUMMY_SERVER_PROTOCOL_H
#define DUMMY_SERVER_PROTOCOL_H

#include "../socket/socket.h"
#include "../commands/move_request.h"
#include "../utils/vector_2d.h"
#include "../../server/types.h"

#include <vector>

class DummyServerProtocol {
private:
    Socket& skt;

public:
    /**
     * Constructor: takes a Socket reference to use for communication
     */
    explicit DummyServerProtocol(Socket& socket);

    /**
     * Receives a move request from the client.
     */
    MoveRequest recv_move_request();

    /**
     * Sends a positions response to the client.
     */
    void send_positions_response(const std::vector<std::pair<ClientID, Vector2D>>& positions);

    /**
     * Destructor
     * Nothing special to do
     */
    ~DummyServerProtocol();
};

#endif  // DUMMY_SERVER_PROTOCOL_H
