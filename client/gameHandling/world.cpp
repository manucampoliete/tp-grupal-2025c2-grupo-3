#include "world.h"

void World::reset() {
    std::lock_guard<std::mutex> lock(mtx);
    cars.clear();
    countdown = 0;
}

// Each car is updated according to its id
void World::update(const BroadcastData& data) {
    std::lock_guard<std::mutex> lock(mtx);
    for (const auto& car: data.cars) cars[car.id] = car;
    countdown = data.countdown;
}

std::map<uint8_t, BroadcastData::CarState> World::getCars() const {
    std::lock_guard<std::mutex> lock(mtx);
    return cars;
}

uint32_t World::getCountdown() const {
    std::lock_guard<std::mutex> lock(mtx);
    return countdown;
}
