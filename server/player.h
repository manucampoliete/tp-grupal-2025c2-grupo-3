#ifndef PLAYER_H
#define PLAYER_H

#include "car.h"
#include "types.h"

class Player {
private:
    ClientID client_id;
    std::string username;
    Car car;
public:
    Player(ClientID client_id, const std::string& username, b2Body* body) :
    client_id(client_id), username(username), car(body) {}

    void move(ActiveDirections active_directions);

    void update_car_physics() { car.update_physics(); }

    Snapshot::CarSnapshot build_car_snapshot();
};

#endif

