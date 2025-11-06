#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include "../types/types.h"

#include <cstdint>
#include <vector>

struct Snapshot {
    struct CarSnapshot {
        ClientID id;     // client/player ID
        uint32_t x;      // x coordinate * 1000
        uint32_t y;      // y coordinate * 1000
        uint16_t angle;  // angle in degrees
        uint16_t speed;  // speed (units?)
        CarID carId;     // car type (needed?)

        /**
         * Constructor for CarSnapshot
         */
        CarSnapshot(ClientID id, uint32_t x, uint32_t y, uint16_t angle, uint16_t speed, CarID carId);

        /**
         * Copy constructor for CarSnapshot
         */
        CarSnapshot(const CarSnapshot& other);
    };

    uint16_t countdown;  // remaining race time in milliseconds (or maybe seconds?)
    std::vector<CarSnapshot> cars;

    /**
     * Constructor for Snapshot
     */
    Snapshot(uint16_t countdown, const std::vector<CarSnapshot>& cars);

    /**
     * Copy constructor for Snapshot
     */
    Snapshot(const Snapshot& other);
};

#endif  // SNAPSHOT_H
