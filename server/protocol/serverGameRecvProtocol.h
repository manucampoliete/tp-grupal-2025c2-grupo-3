#ifndef SERVER_GAME_RECV_PROTOCOL_H
#define SERVER_GAME_RECV_PROTOCOL_H

#include "../../common/utils/activeDirections.h"
#include "../requestsResolving/gameResolver.h"

#include "../../common/protocol/recvProtocol.h"

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

    /**
     * Receives an Inmortality message from the socket and
     * processes it using the provided GameResolver.
     */
    void recvInmortality(GameResolver& gameResolver);

    /**
     * Receives an InstaWin message from the socket and
     * processes it using the provided GameResolver.
     */
    void recvInstaWin(GameResolver& gameResolver);

    /**
     * Receives an InstaLose message from the socket and
     * processes it using the provided GameResolver.
     */
    void recvInstaLose(GameResolver& gameResolver);

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
