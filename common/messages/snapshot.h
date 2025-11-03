#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <cstdint>
#include <vector>

struct Snapshot {
    struct CarSnapshot {
        uint16_t id;        // client/player ID
        uint32_t x;         // x coordinate * 1000
        uint32_t y;         // y coordinate * 1000
        uint16_t angle;     // angle in degrees
        uint16_t speed;     // speed (units?)
        uint8_t type;       // car type (needed?)

        CarSnapshot(uint16_t id, uint32_t x, uint32_t y, uint16_t angle, uint16_t speed, uint8_t type)
            : id(id), x(x), y(y), angle(angle), speed(speed), type(type) {}

        CarSnapshot(const CarSnapshot& other)
            : id(other.id), x(other.x), y(other.y), angle(other.angle), speed(other.speed), type(other.type) {}
    };

    uint16_t countdown;     // remaining race time in milliseconds (or maybe seconds?)
    std::vector<CarSnapshot> cars;

    Snapshot(uint16_t countdown, const std::vector<CarSnapshot>& cars)
        : countdown(countdown), cars(cars) {}

    Snapshot(const Snapshot& other)
        : countdown(other.countdown), cars(other.cars) {}
};

#endif // SNAPSHOT_H
