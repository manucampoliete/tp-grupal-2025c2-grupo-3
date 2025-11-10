#include "snapshot.h"

Snapshot::CarSnapshot::CarSnapshot(ClientID id, uint32_t x, uint32_t y, uint16_t angle,
                                   uint16_t speed, CarID carId):
        id(id), x(x), y(y), angle(angle), speed(speed), carId(carId) {}

Snapshot::CarSnapshot::CarSnapshot(const CarSnapshot& other):
        id(other.id),
        x(other.x),
        y(other.y),
        angle(other.angle),
        speed(other.speed),
        carId(other.carId) {}

Snapshot::Snapshot(): countdown(0), cars(), type(SnapshotType::START_SIGNAL) {}

Snapshot::Snapshot(uint32_t countdown, const std::vector<CarSnapshot>& cars):
        countdown(countdown), cars(cars), type(SnapshotType::CAR_SNAPSHOT) {}

Snapshot::Snapshot(const Snapshot& other): countdown(other.countdown), cars(other.cars), type(other.type) {}
