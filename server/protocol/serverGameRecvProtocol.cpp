#include "serverGameRecvProtocol.h"

#include "../../common/protocol/protocolConstants.h"

ActiveDirections ServerGameRecvProtocol::decodeMoveState(uint8_t moveState) {
    return ActiveDirections(moveState & UP_MASK, moveState & DOWN_MASK, moveState & LEFT_MASK,
                            moveState & RIGHT_MASK);
}

void ServerGameRecvProtocol::recvMoveState(GameResolver& gameResolver) {
    gameResolver.handleMove(decodeMoveState(recvU8()));
}

void ServerGameRecvProtocol::recvInmortality(GameResolver& gameResolver) {
    gameResolver.handleInmortality();
}

void ServerGameRecvProtocol::recvInstaWin(GameResolver& gameResolver) {
    gameResolver.handleInstaWin();
}

void ServerGameRecvProtocol::recvInstaLose(GameResolver& gameResolver) {
    gameResolver.handleInstaLose();
}

void ServerGameRecvProtocol::recvModifyCar(GameResolver& gameResolver) {
    gameResolver.handleModifyCar(recvU8() == 0x01, recvU8() == 0x01);
}

ServerGameRecvProtocol::ServerGameRecvProtocol(Socket& skt): RecvProtocol(skt) {}

void ServerGameRecvProtocol::consumeOne(GameResolver& gameResolver) {
    switch (recvU8()) {
        case SEND_MOVE_STATE: {
            recvMoveState(gameResolver);
            break;
        }
        case SEND_INMORTALITY: {
            recvInmortality(gameResolver);
            break;
        }
        case SEND_INSTA_WIN: {
            recvInstaWin(gameResolver);
            break;
        }
        case SEND_INSTA_LOSE: {
            recvInstaLose(gameResolver);
            break;
        }
        case MSG_MODIFY_CAR: {
            recvModifyCar(gameResolver);
            break;
        }
        default:
            /**
             * TODO: Handle unknown message type
             */
            break;
    }
}
