#ifndef DUMMY_CLIENT_PROTOCOL_H
#define DUMMY_CLIENT_PROTOCOL_H

#include <string>
#include <vector>

#include "../socket/socket.h"
#include "../commands/move_request.h"
#include "../utils/vector_2d.h"
#include "../../server/types.h"
#include "serializer.h"

class DummyClientProtocol {
private:
    Socket& skt;
    Serializer serializer;

public:
    /**
     * Constructor: takes a Socket reference to use for communication
     */
    explicit DummyClientProtocol(Socket& socket);

    /**
     * Sends a move request to the server.
     */
    void send_move_request(const MoveRequest& request);

    std::vector<std::pair<ClientID, Vector2D>> recv_positions_response();

    void send_modifications(bool mod_speed, bool mod_accel);

    /**
     * Destructor
     * Nothing special to do
     */
    ~DummyClientProtocol();
};

#endif  // DUMMY_CLIENT_PROTOCOL_H
