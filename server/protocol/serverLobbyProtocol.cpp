#include "serverLobbyProtocol.h"
#include "../../common/protocol/protocolConstants.h"

#include <string>
#include <vector>

void ServerLobbyProtocol::sendClientID(ClientID clientId) {
    sendU8(SEND_CLIENT_ID);
    sendU16(clientId);
}

void ServerLobbyProtocol::sendInitialInfo() {
    const auto& carsInfo = lobbyResolver.getCarsInfo();

    sendU8(SEND_INITIAL_INFO);
    sendU16(carsInfo.size());

    for (auto info: carsInfo) {
        sendU8(info.id);
        sendString(info.name);
        sendU16(static_cast<uint16_t>(info.maxSpeed));
        sendU16(static_cast<uint16_t>(info.health));
    }
}

void ServerLobbyProtocol::recvCreateMatch() {
    std::string username = recvString();
    CarID carId = recvU8();
    lobbyResolver.handleCreateMatch(username, carId, *this);
}

void ServerLobbyProtocol::recvJoinMatch() {
    MatchID matchId = recvU16();
    std::string username = recvString();
    CarID carId = recvU8();
    lobbyResolver.handleJoinMatch(matchId, username, carId, *this);
}

void ServerLobbyProtocol::recvStartMatch() {
    lobbyResolver.handleStartMatch(*this);
}

ServerLobbyProtocol::ServerLobbyProtocol(Socket& socket, LobbyResolver& lobbyResolver, ClientID clientId):
        RecvProtocol(socket), SendProtocol(socket), lobbyResolver(lobbyResolver) {
    sendClientID(clientId);
}

void ServerLobbyProtocol::consumeOne() {
    switch (recvU8()) {
        case SEND_INITIAL_INFO: {
            sendInitialInfo();
            break;
        }
        case SEND_CREATE: {
            recvCreateMatch();
            break;
        }
        case SEND_JOIN: {
            recvJoinMatch();
            break;
        }
        case SEND_START: {
            recvStartMatch();
            break;
        }
        default: {
            /**
             * If we are here it is because recvU8() returned an unknown message type
             * NOTE: the case of disconnection is handled at RecvProtocol level
             */
            throw std::runtime_error("Lobby phase aborted due to unknown message type");
            break;
        }
    }
}

void ServerLobbyProtocol::sendCreated(MatchID matchId) {
    sendU8(SEND_CREATED);
    sendU16(matchId);
}

void ServerLobbyProtocol::sendJoined(bool success) {
    sendU8(SEND_JOINED);
    sendU8(success ? 0x00 : 0x01);
}

void ServerLobbyProtocol::sendStarted(bool success) {
    sendU8(SEND_STARTED);
    sendU8(success ? 0x00 : 0x01);
}
