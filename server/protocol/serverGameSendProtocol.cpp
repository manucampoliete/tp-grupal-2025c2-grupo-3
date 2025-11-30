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
        sendU8(car.health);
        sendU8(car.onBridge ? 1 : 0);

        sendU8(car.path.size());
        for (auto& element : car.path) {
            sendU8(element.id);
            sendU32(element.x);
            sendU32(element.y);
        }
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

void ServerGameSendProtocol::sendPlayerDiedSnapshot(ClientID id) {
    sendU16(id);
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

void ServerGameSendProtocol::sendFinalResultsSnapshot(std::shared_ptr<Snapshot> snapshot) {
    Snapshot::FinalResults& fr = snapshot->finalResults; 
    sendU16(fr.standings.size());

    for (auto& fs : fr.standings) {
        sendU16(fs.playerId);
        sendString(fs.playerName);
        sendU32(fs.totalTimeMs);
        sendU8(fs.position);
    }
    sendU16(fr.winnerId);
    sendString(fr.winnerName);
}

void ServerGameSendProtocol::sendModificationSnapshot(std::shared_ptr<Snapshot> snapshot) {
    uint8_t numPlayers = static_cast<uint8_t>(snapshot->carProperties.size());
    sendU8(numPlayers);

    for (const auto& prop: snapshot->carProperties) {
        sendU16(prop.playerId);
        sendU16(prop.speed);
        sendU16(prop.health);
        sendU16(prop.acceleration);
        sendU16(prop.mass);
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
        case MSG_PLAYER_DIED:
            sendPlayerDiedSnapshot(snapshot->clientId);
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
            sendFinalResultsSnapshot(snapshot);
            break;
    }
}
