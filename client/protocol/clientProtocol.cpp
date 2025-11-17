#include "clientProtocol.h"

#include <iostream>
#include <stdexcept>

#include <arpa/inet.h>

#include "../../common/protocol/protocolConstants.h"

uint8_t ClientProtocol::encodeMoveState(const ActiveDirections& activeDirections) {
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


ClientProtocol::ClientProtocol(Socket& socket): SendProtocol(socket), RecvProtocol(socket) {}


/**
 * HANDSHAKE
 */
ClientID ClientProtocol::recvClientId() {
    if (recvU8() != SEND_CLIENT_ID)
        throw std::runtime_error("Expected SEND_CLIENT_ID response from server");
    return recvU16();  // client ID
}

std::vector<CarInfo> ClientProtocol::recvInitialInfo() {
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


/**
 * LOBBY
 */
uint16_t ClientProtocol::sendCreate(const std::string& username, CarID carId) {
    sendU8(SEND_CREATE);
    sendString(username);
    sendU8(carId);

    if (recvU8() != SEND_CREATED) {
        throw std::runtime_error("Expected SEND_CREATED response from server");
    }
    return recvU16();  // match ID
}

bool ClientProtocol::sendJoin(MatchID matchId, const std::string& username, CarID carId) {
    sendU8(SEND_JOIN);
    sendU16(matchId);
    sendString(username);
    sendU8(carId);

    if (recvU8() != SEND_JOINED) {
        throw std::runtime_error("Expected SEND_JOINED response from server");
    }
    return recvU8() == 0x00;  // 0x00 for success, 0x01 for failure
}

void ClientProtocol::sendStart() {
    sendU8(SEND_START);
}

void ClientProtocol::recvStartSignal() {
    if (recvU8() != SEND_STARTED) {
        throw std::runtime_error("Expected SEND_STARTED signal from server");
    }
}


/**
 * GAME
 */
uint8_t ClientProtocol::recvMessageType() { return recvU8(); }

void ClientProtocol::sendMove(const ActiveDirections& activeDirections) {
    sendU8(SEND_MOVE_STATE);
    sendU8(encodeMoveState(activeDirections));
}

void ClientProtocol::sendModifications(bool speedMod, bool healthMod) {
    sendU8(MSG_MODIFY_CAR);
    sendU8(speedMod ? 0x01 : 0x00);
    sendU8(healthMod ? 0x01 : 0x00);
}

Snapshot ClientProtocol::recvSnapshot() {
    uint32_t countdown = recvU32();
    uint8_t numCars = recvU8();

    std::cout << "[PROTOCOL] Snapshot received: countdown=" << countdown
              << "ms, numCars=" << numCars << std::endl;

    std::vector<Snapshot::CarSnapshot> cars;
    for (uint8_t i = 0; i < numCars; ++i) {
        ClientID clientId = recvU16();
        uint32_t x = recvU32();
        uint32_t y = recvU32();
        uint16_t angle = recvU16();
        uint16_t speed = recvU16();
        CarID carId = recvU8();

        std::cout << "  [Car " << i << "] clientId=" << clientId << ", pos=(" << x << "," << y << ")"
                  << ", angle=" << angle << ", speed=" << speed << ", carId=" << (int)carId
                  << std::endl;

        cars.emplace_back(clientId, x, y, angle, speed, carId);
    }

    return Snapshot(countdown, cars);
}

uint8_t ClientProtocol::recvCountdown() { return recvU8(); }

uint8_t ClientProtocol::recvCheckpoint() { return recvU8(); }

CollisionData ClientProtocol::recvCollision() {
    CollisionData collision;
    collision.player_id = recvU16();
    
    // Receive intensity as uint8_t (0-255) and convert to float (0.0-1.0)
    uint8_t intensityByte = recvU8();
    collision.intensity = intensityByte / 255.0f;

    collision.x = recvU32();
    collision.y = recvU32();

    return collision;
}

uint16_t ClientProtocol::recvPlayerDied() { return recvU16(); }

RaceResults ClientProtocol::recvRaceResults() {
    RaceResults results;
    results.countdown_ms = recvU32();
    uint16_t numPlayers = recvU16();

    for (uint16_t i = 0; i < numPlayers; ++i) {
        RaceResults::PlayerResult player;
        player.player_name = recvString();
        player.race_time_ms = recvU32();
        player.total_time_ms = recvU32();

        results.players.push_back(player);
    }

    return results;
}

CarProperties ClientProtocol::recvCarProperties() {
    CarProperties props;

    props.speed = recvU16();
    props.health = recvU16();
    props.countdown_ms = recvU32();

    return props;
}

FinalResults ClientProtocol::recvFinalResults() {
    FinalResults results;
    uint16_t numStandings = recvU16();

    for (uint16_t i = 0; i < numStandings; ++i) {
        FinalResults::FinalStanding standing;
        standing.player_id = recvU16();
        standing.player_name = recvString();
        standing.total_time_ms = recvU32();
        standing.position = recvU8();

        results.standings.push_back(standing);
    }

    results.winner_id = recvU16();
    results.winner_name = recvString();

    return results;
}


/**
 * CHEATS
 */
void ClientProtocol::sendInmortalityRequest() { sendU8(SEND_INMORTALITY); }

void ClientProtocol::sendInstaWinRequest() { sendU8(SEND_INSTA_WIN); }

void ClientProtocol::sendInstaLoseRequest() { sendU8(SEND_INSTA_LOSE); }
