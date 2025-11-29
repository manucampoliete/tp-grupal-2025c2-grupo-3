#include "serverGameRecvProtocol.h"
#include "../../common/protocol/protocolConstants.h"
#include "../../common/errors/peerDisconnectedError.h"

ActiveDirections ServerGameRecvProtocol::decodeMoveState(uint8_t moveState) {
    return ActiveDirections(moveState & UP_MASK, moveState & DOWN_MASK, moveState & LEFT_MASK,
                            moveState & RIGHT_MASK);
}

void ServerGameRecvProtocol::recvMoveState() {
    gameResolver.handleMove(decodeMoveState(recvU8()));
}

void ServerGameRecvProtocol::recvInmortality() {
    gameResolver.handleInmortality();
}

void ServerGameRecvProtocol::recvInstaWin() {
    gameResolver.handleInstaWin();
}

void ServerGameRecvProtocol::recvInstaLose() {
    gameResolver.handleInstaLose();
}

void ServerGameRecvProtocol::recvSuperSpeed() {
    gameResolver.handleSuperSpeed();
}

void ServerGameRecvProtocol::recvModifyCar() {
    bool improveVelocity = recvU8() == 0x01;
    bool improveHealth = recvU8() == 0x01;
    bool improveAcceleration = recvU8() == 0x01;
    bool improveMass = recvU8() == 0x01;

    gameResolver.handleModifyCar(improveVelocity, improveHealth, improveAcceleration, improveMass);
}

void ServerGameRecvProtocol::recvPlayerDisconnected() {
    gameResolver.handlePlayerDisconnected();
}

ServerGameRecvProtocol::ServerGameRecvProtocol(Socket& skt, GameResolver& gameResolver): 
    RecvProtocol(skt), gameResolver(gameResolver) {}

void ServerGameRecvProtocol::consumeOne() {
    try {
        switch (recvU8()) {
            case SEND_MOVE_STATE: {
                recvMoveState();
                break;
            }
            case SEND_INMORTALITY: {
                recvInmortality();
                break;
            }
            case SEND_INSTA_WIN: {
                recvInstaWin();
                break;
            }
            case SEND_INSTA_LOSE: {
                recvInstaLose();
                break;
            }
            case SEND_SUPER_SPEED: {
                recvSuperSpeed();
                break;
            }
            case MSG_MODIFY_CAR: {
                recvModifyCar();
                break;
            }
            default:
                /**
                 * If we are here it is because recvU8() returned an unknown message type
                 */
                throw std::runtime_error("Game phase aborted due to unknown message type");
                break;
        }
    } catch (const PeerDisconnectedError& e) {
        /**
         * Rethrow to be handled at a higher level
         */
        recvPlayerDisconnected();
        throw;
    }
}
