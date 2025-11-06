#include "client_protocol.h"
#include "protocolConstants.h"

#include <stdexcept>
#include <arpa/inet.h>

void ClientProtocol::recvInitialInfo() {
    if (recvU8() != SEND_INITIAL_INFO) {
        throw std::runtime_error("Expected SEND_INITIAL_INFO from server");
    }

    // Deserialize initial info
}

uint8_t ClientProtocol::encodeActiveDirections(const ActiveDirections& directions) {
    uint8_t encoded = 0;
    if (directions.up)    encoded |= UP_MASK;
    if (directions.down)  encoded |= DOWN_MASK;
    if (directions.left)  encoded |= LEFT_MASK;
    if (directions.right) encoded |= RIGHT_MASK;
    return encoded;
}

ClientProtocol::ClientProtocol(Socket& skt)
    : SendProtocol(skt), RecvProtocol(skt) {
    recvInitialInfo();
}

MatchID ClientProtocol::sendCreate(const std::string& username, CarID carId) {
    sendU8(SEND_CREATE);
    sendString(username);
    sendU8(carId);

    if (recvU8() != SEND_CREATED) {
        throw std::runtime_error("Expected SEND_CREATED from server");
    }
    return recvU16();
}

bool ClientProtocol::sendJoin(MatchID matchId, const std::string& username, CarID carId) {
    sendU8(SEND_JOIN);
    sendU16(matchId);
    sendString(username);
    sendU8(carId);

    if (recvU8() != SEND_JOINED) {
        throw std::runtime_error("Expected SEND_JOINED from server");
    }
    return recvU8() == 0x00;  // 0x00 for success, 0x01 for failure
}

bool ClientProtocol::sendStart(MatchID matchId) {
    sendU8(SEND_START);
    sendU16(matchId);

    if (recvU8() != SEND_STARTED) {
        throw std::runtime_error("Expected SEND_STARTED from server");
    }
    return recvU8() == 0x00;  // 0x00 for success, 0x01 for failure
}

void ClientProtocol::sendMove(const ActiveDirections& activeDirections) {
    sendU8(SEND_MOVE_STATE);
    sendU8(encodeActiveDirections(activeDirections));
}

void ClientProtocol::recvStartSignal() {
    if (recvU8() != SEND_STARTED) {
        throw std::runtime_error("Expected SEND_STARTED signal from server");
    }
}
