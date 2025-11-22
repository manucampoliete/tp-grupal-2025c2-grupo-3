#include "player.h"

#include "carBuilder.h"
#include "yaml-cpp/yaml.h"

#define RADTODEG 57.295779513082320876f

Player::Player(ClientID clientId, const std::string& username, b2Body* body, CarID carId):
        clientId(clientId), username(username), car(CarFactory::createCar(body, carId)) {}

void Player::move(ActiveDirections activeDirections) {
    car.updateActiveDirections(activeDirections);
}

void Player::updateCarPhysics() { car.updatePhysics(); }

Snapshot::CarSnapshot Player::buildCarSnapshot() {
    uint32 x = static_cast<uint32_t>(std::round(car.getPosition().x * 1000));
    uint32 y = static_cast<uint32_t>(std::round(car.getPosition().y * 1000));

    // normalizacion del angulo para que este entre 0 y 360
    // evita que en la animacion se vea un "snap" del auto cuando el angulo hace overflow
    float angleDeg = -car.getAngle() * RADTODEG;
    angleDeg = fmodf(angleDeg, 360.0f);
    if (angleDeg < 0.0f)
        angleDeg += 360.0f;
    uint16 angle = static_cast<uint16_t>(std::round(angleDeg));

    uint16_t speed = static_cast<uint16_t>(std::round(car.getSpeed() * 1000));

    return Snapshot::CarSnapshot(clientId, x, y, angle, speed, car.getId());
}
