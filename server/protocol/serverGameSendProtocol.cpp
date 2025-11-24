#include "serverGameSendProtocol.h"

#include <iostream>

#include "../../common/protocol/protocolConstants.h"

ServerGameSendProtocol::ServerGameSendProtocol(Socket& socket): SendProtocol(socket) {}

void ServerGameSendProtocol::sendRaceSnapshot(std::shared_ptr<Snapshot> snapshot) {
    // Send countdown
    sendU32(snapshot->countdown);

    // Send number of cars
    uint8_t numCars = static_cast<uint8_t>(snapshot->cars.size());
    sendU8(numCars);

    // Send each car's snapshot
    for (const auto& car: snapshot->cars) {
        sendU16(car.id);
        sendU32(car.x);
        sendU32(car.y);
        sendU16(car.angle);
        sendU16(car.speed);
        sendU8(car.carId);
    }
}

void ServerGameSendProtocol::sendCollisionSnapshot(const Snapshot::CollisionData& collision) {
    sendU16(collision.playerId);
    // convertir intensidad float (0.0-1.0) a uint8_t (0-255)
    uint8_t intensity = static_cast<uint8_t>(collision.intensity * 255.0f);
    sendU8(intensity);
    sendU32(collision.x);
    sendU32(collision.y);
}

void ServerGameSendProtocol::sendRaceResultsSnapshot(std::shared_ptr<Snapshot> snapshot) {
    uint8_t numPlayers = static_cast<uint8_t>(snapshot->results.players.size());
    sendU8(numPlayers);

    for (const auto& player: snapshot->results.players) {
        sendString(player.playerName);
        sendU32(player.raceTimeMs);
        sendU32(player.totalTimeMs);
    }
}

void ServerGameSendProtocol::sendModificationSnapshot(std::shared_ptr<Snapshot> snapshot) {
    uint8_t numPlayers = static_cast<uint8_t>(snapshot->carProperties.size());
    sendU8(numPlayers);

    for (const auto& prop: snapshot->carProperties) {
        sendU16(prop.playerId);
        sendU16(prop.speed);
        sendU16(prop.health);
    }
}

void ServerGameSendProtocol::sendSnapshot(std::shared_ptr<Snapshot> snapshot) {
    sendU8(snapshot->type);
    switch (snapshot->type) {
        case SEND_STARTED:
            // nada
            break;
        case MSG_RACE_START:
            // nada
            break;
        case MSG_COUNTDOWN:
            sendU8(static_cast<uint8_t>(snapshot->countdown));
            break;
        case SEND_RACE_SNAPSHOT:
            sendRaceSnapshot(snapshot);
            break;
        case MSG_COLLISION:
            sendCollisionSnapshot(snapshot->collisionData);
            break;
        case MSG_RACE_END:  // estoy seria para mostrar las estadisticas
            sendRaceResultsSnapshot(snapshot);
            break;
        case MSG_MOD_PHASE:
            sendModificationSnapshot(snapshot);
            break;
        case MSG_STATS_COUNTDOWN:
        case MSG_MOD_COUNTDOWN:
            sendU8(static_cast<uint8_t>(snapshot->countdown));
            break;
        case MSG_GAME_END:
            // no implementado
            break;
    }
}
