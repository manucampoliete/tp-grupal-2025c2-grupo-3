#include "player.h"

#define RADTODEG 57.295779513082320876f

Player::Player(ClientID clientId, const std::string& username, b2Body* body):
        clientId(clientId), username(username), car(body) {}

void Player::move(ActiveDirections activeDirections) {
    car.updateActiveDirections(activeDirections);
}

void Player::updateCarPhysics() { car.updatePhysics(); }

Snapshot::CarSnapshot Player::buildCarSnapshot() {
    uint32 x = static_cast<uint32_t>(std::round(car.getPosition().x * 1000));
    uint32 y = static_cast<uint32_t>(std::round(car.getPosition().y * 1000));
    uint16 angle = static_cast<uint16_t>(std::round(car.getAngle() * RADTODEG));

    return Snapshot::CarSnapshot(clientId, x, y, angle, 0, 0);  // speed and carId are dummy for now
}
