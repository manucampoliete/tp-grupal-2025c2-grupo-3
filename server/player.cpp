#include "player.h"

#define RADTODEG 57.295779513082320876f

void Player::move(ActiveDirections active_directions) {
    car.update_active_directions(active_directions);
}

Snapshot::CarSnapshot Player::build_car_snapshot() {
    uint32 x = static_cast<uint32_t>(std::round(car.get_position().x * 1000));
    uint32 y = static_cast<uint32_t>(std::round(car.get_position().y * 1000));
    uint16 angle = static_cast<uint16_t>(std::round(car.get_angle() * RADTODEG));

    Snapshot::CarSnapshot snp(client_id, x, y, angle, 0, 0);
}