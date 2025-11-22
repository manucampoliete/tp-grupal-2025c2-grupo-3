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
