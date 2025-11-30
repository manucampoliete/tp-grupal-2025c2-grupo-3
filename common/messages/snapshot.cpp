#include "snapshot.h"

Snapshot::CarSnapshot::CarSnapshot(ClientID id, uint32_t x, uint32_t y, uint16_t angle,
                                   uint16_t speed, CarID carId, uint8_t health, bool onBridge, std::vector<PathElement> path):
        id(id), x(x), y(y), angle(angle), speed(speed), carId(carId), health(health), onBridge(onBridge), path(path) {}

Snapshot::CarSnapshot::CarSnapshot(const CarSnapshot& other):
        id(other.id),
        x(other.x),
        y(other.y),
        angle(other.angle),
        speed(other.speed),
        carId(other.carId),
        health(other.health),
        onBridge(other.onBridge),
        path(other.path) {}

Snapshot::Snapshot(): countdown(0), cars(), type(SEND_STARTED) {}

Snapshot::Snapshot(int type): countdown(0), cars(), type(static_cast<uint8_t>(type)) {} 

Snapshot::Snapshot(uint32_t remaining, int type):
        countdown(remaining), cars(), type(static_cast<uint8_t>(type)) {}

Snapshot::Snapshot(uint32_t countdown, const std::vector<CarSnapshot>& cars):
        countdown(countdown), cars(cars), type(SEND_RACE_SNAPSHOT) {}

Snapshot::Snapshot(const Snapshot& other):
        countdown(other.countdown), cars(other.cars), type(other.type) {}

Snapshot::Snapshot(RaceResults results): results(results), type(MSG_RACE_END) {}

Snapshot::Snapshot(const std::vector<CarProperties>& carProperties): carProperties(carProperties), type(MSG_MOD_PHASE) {}

Snapshot::Snapshot(const CollisionData& collision):
        countdown(0), collisionData(collision), type(MSG_COLLISION) {}

Snapshot::Snapshot(ClientID clientId): 
        clientId(clientId), type(MSG_PLAYER_DIED) {}


Snapshot::CarSnapshot& Snapshot::CarSnapshot::operator=(const CarSnapshot& other) {
    if (this == &other)
        return *this;
    id = other.id;
    x = other.x;
    y = other.y;
    angle = other.angle;
    speed = other.speed;
    carId = other.carId;
    health = other.health;
    onBridge = other.onBridge;
    path = other.path;
    return *this;
}

Snapshot& Snapshot::operator=(const Snapshot& other) {
    if (this == &other)
        return *this;
    countdown = other.countdown;
    cars = other.cars;
    type = other.type;
    return *this;
}