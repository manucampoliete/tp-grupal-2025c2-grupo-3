#ifndef WORLD_H
#define WORLD_H

#include <cstdint>
#include <map>
#include <mutex>
#include <vector>

#include "../utils/gameData.h"


// Maintains the local world state for each client
// Each client has its own World that reflects what the server sends in the broadcasts
class World {
private:
    std::map<uint8_t, BroadcastData::CarState> cars;  // Each car state
    uint32_t countdown = 0;                           // Countdown

public:
    // Cleans world state (for example, when new race is initiated)
    void reset();

    // Updates world state with a new broadcast received from server
    void update(const BroadcastData& data);

    // Obtains a copy of the state of all cars
    std::map<uint8_t, BroadcastData::CarState> getCars() const { return cars; }
    
    // Obtains the remaining time
    uint32_t getCountdown() const { return countdown; }
};

#endif  // WORLD_H
