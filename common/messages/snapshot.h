#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <cstdint>
#include <vector>
#include <string>

#include "../types/types.h"
#include "../protocol/protocolConstants.h"
#include "../utils/pathElements.h"

enum class SnapshotType : uint8_t { START_SIGNAL = 0, GAME_SNAPSHOT = 1 };

struct Snapshot {
    struct CarSnapshot {
        ClientID id;     // client/player ID
        uint32_t x;      // x coordinate * 1000
        uint32_t y;      // y coordinate * 1000
        uint16_t angle;  // angle in degrees
        uint16_t speed;  // speed (units?)
        CarID carId;     // car type (needed?)
        uint8_t health;
        bool onBridge;

        std::vector<PathElement> path;

        /**
         * Constructor for CarSnapshot
         */
        CarSnapshot(ClientID id, uint32_t x, uint32_t y, uint16_t angle, uint16_t speed,
                    CarID carId, uint8_t health, bool onBridge, std::vector<PathElement> path);

        /**
         * Copy constructor for CarSnapshot
         */
        CarSnapshot(const CarSnapshot& other);

        CarSnapshot& operator=(const CarSnapshot& other);
    };

    struct RaceResults {
        struct PlayerResult {
            std::string playerName;
            uint32_t raceTimeMs;
            uint32_t totalTimeMs;
        };

        std::vector<PlayerResult> players;
    };

    struct CarProperties {
        ClientID playerId;
        uint16_t speed;
        uint16_t health;
        uint16_t acceleration;
        uint16_t mass;
    };
    
    struct CollisionData {
        uint16_t playerId;  // quién chocó
        float intensity;     // 0.0 (leve) a 1.0 (fuerte)
                            // Se tiene que mandar como uint8_t (0-255) y el cliente lo pasa a float (0.0-1.0)
        uint32_t x;          // coordenada x del choque * 1000
        uint32_t y;          // coordenada y del choque * 1000
    };

    uint32_t countdown;  // remaining race time in milliseconds
    std::vector<CarSnapshot> cars;

    RaceResults results;

    std::vector<CarProperties> carProperties;
    CollisionData collisionData;
    ClientID clientId; // for dead player snapshot
    
    // SnapshotType type;
    uint8_t type;  // using uint8_t for easier serialization


    /**
     * Constructor for Snapshot
     */
    Snapshot();

    // change state snapshot
    Snapshot(int type);

    // remaining time snapshot (countdown, stats, upgrades)
    Snapshot(uint32_t remaining, int type);

    // racing snapshot
    Snapshot(uint32_t countdown, const std::vector<CarSnapshot>& cars);

    // stats snapshot
    Snapshot(RaceResults results);

    // mod snapshot
    Snapshot(const std::vector<Snapshot::CarProperties>& carProperties);
    
    // collision snapshot
    Snapshot(const CollisionData& collision);

    // dead player snapshot
    Snapshot(ClientID clientId);

    /**
     * Copy constructor for Snapshot
     */
    Snapshot(const Snapshot& other);

    Snapshot& operator=(const Snapshot& other);
};

#endif  // SNAPSHOT_H
