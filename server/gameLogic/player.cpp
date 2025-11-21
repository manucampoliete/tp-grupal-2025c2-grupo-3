#include "player.h"

#include "car_builder.h"
#include "yaml-cpp/yaml.h"

#include "bodyData.h"


#define RADTODEG 57.295779513082320876f

Player::Player(ClientID clientId, const std::string& username, b2Body* body, CarID carId):
        clientId(clientId), username(username), car(CarBuilder::createCar(body, carId)) 
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

Snapshot::CollisionData Player::buildCollisionSnapshot(float impact) {
    Snapshot::CollisionData collision;
    collision.playerId = clientId;

    // clamp impact between 0.0 and 1.0
    if (impact < 0.0f)
        impact = 0.0f;
    else if (impact > 1.0f)
        impact = 1.0f;

    collision.intensity = impact;

    uint32 x = static_cast<uint32_t>(std::round(car.getPosition().x * 1000));
    uint32 y = static_cast<uint32_t>(std::round(car.getPosition().y * 1000));

    collision.x = x;
    collision.y = y;

    return collision;
}
