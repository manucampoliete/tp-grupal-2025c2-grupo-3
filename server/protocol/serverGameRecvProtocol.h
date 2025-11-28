#ifndef SERVER_GAME_RECV_PROTOCOL_H
#define SERVER_GAME_RECV_PROTOCOL_H

#include "../../common/protocol/recvProtocol.h"
#include "../../common/utils/activeDirections.h"
#include "../requestsResolving/gameResolver.h"

class ServerGameRecvProtocol: public RecvProtocol {
private:
    GameResolver& gameResolver;

    /**
     * Decodes a Move State byte into an ActiveDirections object.
     */
    ActiveDirections decodeMoveState(uint8_t moveState);

    /**
     * Receives a Move State message from the socket and
     * processes it using the GameResolver.
     */
    void recvMoveState();

    /**
     * Receives an Inmortality message from the socket and
     * processes it using the GameResolver.
     */
    void recvInmortality();

    /**
     * Receives an InstaWin message from the socket and
     * processes it using the GameResolver.
     */
    void recvInstaWin();

    /**
     * Receives an InstaLose message from the socket and
     * processes it using the GameResolver.
     */
    void recvInstaLose();

    /**
     * Receives a SuperSpeed message from the socket and
     * processes it using the GameResolver.
     */
    void recvSuperSpeed();

    /**
     * Receives a ModifyCar message from the socket and
     * processes it using the GameResolver.
     */
    void recvModifyCar();

    /**
     * Receives a PlayerDisconnect message from the socket and
     * processes it using the GameResolver.
     */
    void recvPlayerDisconnected();

public:
    /**
     * Constructor that takes a reference to a Socket object and a GameResolver object.
     */
    explicit ServerGameRecvProtocol(Socket& skt, GameResolver& gameResolver);

    /**
     * Consumes one protocol message from the socket and
     * processes it using the GameResolver.
     */
    void consumeOne();
};

#endif  // SERVER_GAME_RECV_PROTOCOL_H
