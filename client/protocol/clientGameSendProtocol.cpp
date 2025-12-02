#include "clientGameSendProtocol.h"
#include "../../common/protocol/protocolConstants.h"

uint8_t ClientGameSendProtocol::encodeMoveState(const ActiveDirections& activeDirections) {
    uint8_t moveState = 0;
    if (activeDirections.up)
        moveState |= UP_MASK;
    if (activeDirections.down)
        moveState |= DOWN_MASK;
    if (activeDirections.left)
        moveState |= LEFT_MASK;
    if (activeDirections.right)
        moveState |= RIGHT_MASK;
    return moveState;
}

ClientGameSendProtocol::ClientGameSendProtocol(Socket& skt) :
    SendProtocol(skt) {}

void ClientGameSendProtocol::sendMove(const ActiveDirections& activeDirections) {
    sendU8(SEND_MOVE_STATE);
    sendU8(encodeMoveState(activeDirections));
}

void ClientGameSendProtocol::sendModifications(bool speedMod, bool healthMod, bool accelMod, bool massMod) {
    sendU8(MSG_MODIFY_CAR);
    sendU8(speedMod ? MOD_ACTIVATED : MOD_DEACTIVATED);
    sendU8(healthMod ? MOD_ACTIVATED : MOD_DEACTIVATED);
    sendU8(accelMod ? MOD_ACTIVATED : MOD_DEACTIVATED);
    sendU8(massMod ? MOD_ACTIVATED : MOD_DEACTIVATED);
}

/**
 * CHEATS
 */
void ClientGameSendProtocol::sendInmortalityRequest() { 
    sendU8(SEND_INMORTALITY); 
}

void ClientGameSendProtocol::sendInstaWinRequest() { 
    sendU8(SEND_INSTA_WIN); 
}

void ClientGameSendProtocol::sendInstaLoseRequest() { 
    sendU8(SEND_INSTA_LOSE); 
}

void ClientGameSendProtocol::sendSuperSpeedRequest() { 
    sendU8(SEND_SUPER_SPEED); 
}
