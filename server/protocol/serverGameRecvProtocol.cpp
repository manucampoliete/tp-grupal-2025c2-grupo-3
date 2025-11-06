#include "serverGameRecvProtocol.h"

#include "protocolConstants.h"

ActiveDirections ServerGameRecvProtocol::decodeMoveState(uint8_t moveState) {
    return ActiveDirections(moveState & UP_MASK, moveState & DOWN_MASK, moveState & LEFT_MASK,
                            moveState & RIGHT_MASK);
}

void ServerGameRecvProtocol::recvMoveState(GameResolver& gameResolver) {
    uint8_t moveState = recvU8();
    gameResolver.handleMove(decodeMoveState(moveState));
}

explicit ServerGameRecvProtocol::ServerGameRecvProtocol(Socket& skt): RecvProtocol(skt) {}

void ServerGameRecvProtocol::consumeOne(GameResolver& gameResolver) {
    switch (recvU8()) {
        case SEND_MOVE_STATE: {
            recvMoveState(gameResolver);
            break;
        }
        default:
            /**
             * TODO: Handle unknown message type
             */
            break;
    }
}
