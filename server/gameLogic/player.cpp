#include "player.h"

#include "carBuilder.h"
#include "yaml-cpp/yaml.h"

#include "bodyData.h"

#include <iostream>


#define RADTODEG 57.295779513082320876f

Player::Player(ClientID clientId, const std::string& username, b2Body* body, CarID carId):
        clientId(clientId), username(username), car(CarBuilder::createCar(body, carId)), totalRaceTime(0), penalty(0)
{
    auto* data = new BodyData();
    data->player = this;

    // al parecer esta es la forma moderna de hacerlo
    // SetUserData es de versiones viejas de box2d
    body->GetUserData().pointer = reinterpret_cast<uintptr_t>(data);
}

void Player::move(ActiveDirections activeDirections) {
    car.updateActiveDirections(activeDirections);
}

void Player::updateCarPhysics() { car.updatePhysics(); }

bool Player::hasFinished() { return finished; }

void Player::setArrivalTime(float arrivalTime) {
    if (!finished) {
        finished = true;
        currentRaceTime = static_cast<uint32_t>(std::round(arrivalTime * 1000));
        // sumar penalty si existe
        currentRaceTime += (penalty > 0) ? (penalty * 1000) : 0;
        totalRaceTime += currentRaceTime;
        penalty = 0;
    }
}

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

    uint16_t speed = static_cast<uint16_t>(std::round(car.getCurrentSpeed() * 1000));

    uint8_t healthPercentage = static_cast<uint8_t>(std::round((car.getCurrentHealth() / car.getMaxHealth()) * 100));

    return Snapshot::CarSnapshot(clientId, x, y, angle, speed, car.getId(), healthPercentage, car.isOnBridge());
}

void Player::improveCarProperties(bool improveVelocity, bool improveHealth)  {
    car.improveProperties(improveVelocity, improveHealth);
    penalty = 0;
    penalty += (improveVelocity ? PENALTY_PER_IMPROVEMENT : 0) +
                (improveHealth ? PENALTY_PER_IMPROVEMENT : 0);
}

void Player::resetForNewRace() {
    finished = false;
    currentRaceTime = 0;
    car.setCurrentHealth(car.getMaxHealth());
}
// espera un impacto normalizado
Snapshot::CollisionData Player::buildCollisionSnapshot(float normalizedImpact) {
    Snapshot::CollisionData collision;
    collision.playerId = clientId;

    collision.intensity = normalizedImpact;

    uint32 x = static_cast<uint32_t>(std::round(car.getPosition().x * 1000));
    uint32 y = static_cast<uint32_t>(std::round(car.getPosition().y * 1000));

    collision.x = x;
    collision.y = y;

    return collision;
}
