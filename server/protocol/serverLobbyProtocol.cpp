#include "serverLobbyProtocol.h"

#include <string>
#include <vector>

#include <yaml-cpp/yaml.h>

#include "../../common/protocol/protocolConstants.h"
#include "../../common/utils/carinfo.h"

void ServerLobbyProtocol::sendClientID(ClientID clientId) {
    sendU8(SEND_CLIENT_ID);
    sendU16(clientId);
}

void ServerLobbyProtocol::sendInitialInfo() {
    YAML::Node config = YAML::LoadFile("config.yaml");

    CarID carId = 0;
    YAML::Node carInfo = config["cars"][static_cast<int>(carId)];
    std::vector<CarInfo> infos;
    while (carInfo) {
        CarInfo info;
        info.id = carId;
        info.name = carInfo["name"].as<std::string>();

        float max_speed = carInfo["max_speed"].as<float>();
        info.speed = static_cast<uint16_t>(std::round(max_speed));

        float health = carInfo["health"].as<float>();
        info.health = static_cast<uint16_t>(std::round(health));

        infos.emplace_back(info);

        carInfo = config["cars"][static_cast<int>(++carId)];
    }

    sendU8(SEND_INITIAL_INFO);
    sendU16(infos.size());

    for (auto info: infos) {
        sendU8(info.id);
        sendString(info.name);
        sendU16(info.speed);
        sendU16(info.health);
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
