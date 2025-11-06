#ifndef SERVER_GAME_RECV_PROTOCOL_H
#define SERVER_GAME_RECV_PROTOCOL_H

#include "../../common/utils/activeDirections.h"
#include "../requestsResolving/gameResolver.h"

#include "recvProtocol.h"

class ServerGameRecvProtocol: public RecvProtocol {
private:
    /**
     * Decodes a Move State byte into an ActiveDirections object.
     */
    ActiveDirections decodeMoveState(uint8_t moveState);

    /**
     * Receives a Move State message from the socket and
     * processes it using the provided GameResolver.
     */
    void recvMoveState(GameResolver& gameResolver);

public:
    /**
     * Constructor that takes a reference to a Socket object.
     */
    explicit ServerGameRecvProtocol(Socket& skt);

    /**
     * Consumes one protocol message from the socket and
     * processes it using the provided GameResolver.
     */
    void consumeOne(GameResolver& gameResolver);
};

#endif  // SERVER_GAME_RECV_PROTOCOL_H
