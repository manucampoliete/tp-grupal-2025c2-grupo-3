#include "clientLobbyProtocol.h"
#include "../../common/protocol/protocolConstants.h"
#include <stdexcept>

ClientLobbyProtocol::ClientLobbyProtocol(Socket& skt)
    : SendProtocol(skt), RecvProtocol(skt) {}

std::vector<CarInfo> ClientLobbyProtocol::recvInitialInfo() {
    sendU8(SEND_INITIAL_INFO);
    if (recvU8() != SEND_INITIAL_INFO) {
        throw std::runtime_error("Expected SEND_INITIAL_INFO response from server");
    }

    std::vector<CarInfo> infos;
    uint16_t size = recvU16();
    for (int i = 0; i < size; i++) {
        CarInfo info;
        info.id = recvU8();
        info.name = recvString();
        info.speed = recvU16();
        info.health = recvU16();

        infos.emplace_back(info);
    }
    return infos;
}

uint16_t ClientLobbyProtocol::sendCreate(const std::string& username, CarID carId) {
    sendU8(SEND_CREATE);
    sendString(username);
    sendU8(carId);

    if (recvU8() != SEND_CREATED) {
        throw std::runtime_error("Expected SEND_CREATED response from server");
    }
    return recvU16();  // match ID
}

bool ClientLobbyProtocol::sendJoin(MatchID matchId, const std::string& username, CarID carId) {
    sendU8(SEND_JOIN);
    sendU16(matchId);
    sendString(username);
    sendU8(carId);

    if (recvU8() != SEND_JOINED) {
        throw std::runtime_error("Expected SEND_JOINED response from server");
    }
    return recvU8() == 0x00;  // 0x00 for success, 0x01 for failure
}

void ClientLobbyProtocol::sendStart() {
    sendU8(SEND_START);
}

void ClientLobbyProtocol::recvStartSignal() {
    if (recvU8() != SEND_STARTED) {
        throw std::runtime_error("Expected SEND_STARTED signal from server");
    }
}
