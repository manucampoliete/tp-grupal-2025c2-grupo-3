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

void ClientGameProtocol::sendModifications(bool speedMod, bool healthMod, bool accelMod, bool massMod) {
    sendU8(MSG_MODIFY_CAR);
    sendU8(speedMod ? 0x01 : 0x00);
    sendU8(healthMod ? 0x01 : 0x00);
    sendU8(accelMod ? 0x01 : 0x00);
    sendU8(massMod ? 0x01 : 0x00);
}


Snapshot ClientGameProtocol::recvSnapshot() {
    uint32_t countdown = recvU32();
    uint8_t numCars = recvU8();

    std::vector<Snapshot::CarSnapshot> cars;
    for (uint8_t i = 0; i < numCars; ++i) {
        ClientID clientId = recvU16();
        uint32_t x = recvU32();
        uint32_t y = recvU32();
        uint16_t angle = recvU16();
        uint16_t speed = recvU16();
        CarID carId = recvU8();
        uint8_t health = recvU8();
        bool onBridge = recvU8();

        uint8_t numElements = recvU8();
        std::vector<PathElement> pathElements;

        for (uint8_t j = 0; j < numElements; ++j) {
            uint8_t cpId = recvU8();
            uint32_t cpX = recvU32();
            uint32_t cpY = recvU32();
            pathElements.emplace_back(cpId, cpX, cpY);
        }

        cars.emplace_back(clientId, x, y, angle, speed, carId, health, onBridge, pathElements);
    }

    return Snapshot(countdown, cars);
}

RaceStart ClientGameProtocol::recvRaceStart() {
    RaceStart info;
    info.mapId = recvU8();
    info.race = recvU8();
    info.totalRaces = recvU8();

    return info;
}

uint8_t ClientGameProtocol::recvCountdown() { return recvU8(); }

CollisionData ClientGameProtocol::recvCollision() {
    CollisionData collision;
    collision.playerId = recvU16();
    collision.intensity = recvU8(); // 0: low, 1: high
    collision.x = recvU32();
    collision.y = recvU32();

    return collision;
}

uint16_t ClientGameProtocol::recvPlayerDied() { return recvU16(); }

RaceResults ClientGameProtocol::recvRaceResults() {
    RaceResults results;
    uint8_t numPlayers = recvU8();

    for (uint8_t i = 0; i < numPlayers; ++i) {
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

std::vector<CarProperties> ClientGameProtocol::recvCarProperties() {
    uint8_t numCars = recvU8();

    std::vector<CarProperties> props;
    for (uint8_t i = 0; i < numCars; ++i) {
        CarProperties prop;
        prop.playerId = recvU16();
        prop.speed = recvU16();
        prop.health = recvU16();
        prop.accel = recvU16();
        prop.mass = recvU16();

        props.push_back(prop);
    }

    return props;
}

uint8_t ClientGameProtocol::recvModCountdown() {
    return recvU8();
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

void ClientGameProtocol::sendSuperSpeedRequest() { sendU8(SEND_SUPER_SPEED); }
