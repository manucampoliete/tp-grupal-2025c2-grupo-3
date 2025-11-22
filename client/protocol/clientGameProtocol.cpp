#include "clientGameProtocol.h"

#include <iostream>
#include <stdexcept>

#include <arpa/inet.h>

#include "../../common/protocol/protocolConstants.h"

uint8_t ClientGameProtocol::encodeMoveState(const ActiveDirections& activeDirections) {
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

ClientGameProtocol::ClientGameProtocol(Socket& socket): SendProtocol(socket), RecvProtocol(socket) {}

uint8_t ClientGameProtocol::recvMessageType() { return recvU8(); }

void ClientGameProtocol::sendMove(const ActiveDirections& activeDirections) {
    sendU8(SEND_MOVE_STATE);
    sendU8(encodeMoveState(activeDirections));
}

void ClientGameProtocol::sendModifications(bool speedMod, bool healthMod) {
    sendU8(MSG_MODIFY_CAR);
    sendU8(speedMod ? 0x01 : 0x00);
    sendU8(healthMod ? 0x01 : 0x00);
}

Snapshot ClientGameProtocol::recvSnapshot() {
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

uint8_t ClientGameProtocol::recvCountdown() { return recvU8(); }

uint8_t ClientGameProtocol::recvCheckpoint() { return recvU8(); }

CollisionData ClientGameProtocol::recvCollision() {
    CollisionData collision;
    collision.playerId = recvU16();
    
    // Receive intensity as uint8_t (0-255) and convert to float (0.0-1.0)
    uint8_t intensityByte = recvU8();
    collision.intensity = intensityByte / 255.0f;

    collision.x = recvU32();
    collision.y = recvU32();

    return collision;
}

uint16_t ClientGameProtocol::recvPlayerDied() { return recvU16(); }

RaceResults ClientGameProtocol::recvRaceResults() {
    RaceResults results;
    uint16_t numPlayers = recvU16();

    for (uint16_t i = 0; i < numPlayers; ++i) {
        RaceResults::PlayerResult player;
        player.playerName = recvString();
        player.raceTimeMs = recvU32();
        player.totalTimeMs = recvU32();

        results.players.push_back(player);
    }

    return results;
}

uint8_t ClientGameProtocol::recvStatsCountdown() {
    return recvU8();
}

CarProperties ClientGameProtocol::recvCarProperties() {
    CarProperties props;

    props.speed = recvU16();
    props.health = recvU16();
    props.countdownMs = recvU32();

    return props;
}

FinalResults ClientGameProtocol::recvFinalResults() {
    FinalResults results;
    uint16_t numStandings = recvU16();

    for (uint16_t i = 0; i < numStandings; ++i) {
        FinalResults::FinalStanding standing;
        standing.playerId = recvU16();
        standing.playerName = recvString();
        standing.totalTimeMs = recvU32();
        standing.position = recvU8();

        results.standings.push_back(standing);
    }

    results.winnerId = recvU16();
    results.winnerName = recvString();

    return results;
}


/**
 * CHEATS
 */
void ClientGameProtocol::sendInmortalityRequest() { sendU8(SEND_INMORTALITY); }

void ClientGameProtocol::sendInstaWinRequest() { sendU8(SEND_INSTA_WIN); }

void ClientGameProtocol::sendInstaLoseRequest() { sendU8(SEND_INSTA_LOSE); }
