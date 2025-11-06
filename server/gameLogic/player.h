#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <utility>

#include "car.h"
#include "../common/messages/snapshot.h"

class Player {
private:
    ClientID clientId;
    std::string username;
    Car car;

public:
    Player(ClientID clientId, const std::string& username, b2Body* body);

    void move(ActiveDirections activeDirections);

    void updateCarPhysics();

    Snapshot::CarSnapshot buildCarSnapshot();
};

#endif
