#include "serverLobbyProtocol.h"

#include <string>

#include "../../common/protocol/protocolConstants.h"

void ServerLobbyProtocol::recvCreateMatch(LobbyResolver& lobbyResolver) {
    std::string username = recvString();
    CarID carId = recvU8();
    lobbyResolver.handleCreateMatch(username, carId, *this);
}

void ServerLobbyProtocol::recvJoinMatch(LobbyResolver& lobbyResolver) {
    MatchID matchId = recvU16();
    std::string username = recvString();
    CarID carId = recvU8();
    lobbyResolver.handleJoinMatch(matchId, username, carId, *this);
}

void ServerLobbyProtocol::recvStartMatch(LobbyResolver& lobbyResolver) {
    lobbyResolver.handleStartMatch(*this);
}

ServerLobbyProtocol::ServerLobbyProtocol(Socket& socket):
        RecvProtocol(socket), SendProtocol(socket) {}

void ServerLobbyProtocol::consumeOne(LobbyResolver& lobbyResolver) {
    switch (recvU8()) {
        case SEND_CREATE: {
            recvCreateMatch(lobbyResolver);
            break;
        }
        case SEND_JOIN: {
            recvJoinMatch(lobbyResolver);
            break;
        }
        case SEND_START: {
            recvStartMatch(lobbyResolver);
            break;
        }
        default:
            /**
             * TODO: Handle unknown message type
             */
            break;
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
