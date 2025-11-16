#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <cstdint>
#include <vector>

#include "../types/types.h"
#include "../protocol/protocolConstants.h"

enum class SnapshotType : uint8_t { START_SIGNAL = 0, GAME_SNAPSHOT = 1 };

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
        CarSnapshot(ClientID id, uint32_t x, uint32_t y, uint16_t angle, uint16_t speed,
                    CarID carId);

        /**
         * Copy constructor for CarSnapshot
         */
        CarSnapshot(const CarSnapshot& other);

        CarSnapshot& operator=(const CarSnapshot& other);
    };

    uint32_t countdown;  // remaining race time in milliseconds
    std::vector<CarSnapshot> cars;
    
    // SnapshotType type;
    uint8_t type;  // using uint8_t for easier serialization


    /**
     * Constructor for Snapshot
     */
    Snapshot();

    // countdown snapshot
    Snapshot(uint32_t countdown);

    // racing snapshot
    Snapshot(uint32_t countdown, const std::vector<CarSnapshot>& cars);

    /**
     * Copy constructor for Snapshot
     */
    Snapshot(const Snapshot& other);

    Snapshot& operator=(const Snapshot& other);
};

#endif  // SNAPSHOT_H
