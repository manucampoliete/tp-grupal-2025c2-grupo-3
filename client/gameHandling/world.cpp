#include "world.h"


void World::reset() {
    cars.clear();
    countdown = 0;
}

// Each car is updated according to its id
void World::update(const BroadcastData& data) {
    for (const auto& car: data.cars) cars[car.id] = car;
    countdown = data.countdown;
}

uint8_t World::getPlayerHealth(uint8_t playerId) const {
    auto it = cars.find(playerId);
    if (it != cars.end()) 
        return it->second.health;
    return 0;
}

bool World::hasPlayerFinished(uint8_t playerId) const {
    auto it = cars.find(playerId);
    if (it != cars.end()) {
        return it->second.checkpoints.empty();
    }
    return false;
}
