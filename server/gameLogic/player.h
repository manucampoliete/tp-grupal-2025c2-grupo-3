#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>
#include <utility>

#include "../../common/messages/snapshot.h"
#include "cars/car.h"

class Player {
private:
    ClientID clientId;
    std::string username;
    Car car;


public:
    /**
     * Constructor
     */
    Player(ClientID clientId, const std::string& username, b2Body* body, CarID carId);

    /**
     * Updates the active directions of the player's car.
     */
    void move(ActiveDirections activeDirections);

    /**
     * Updates the physics of the player's car.
     */
    void updateCarPhysics();

    /**
     * Builds and returns a CarSnapshot representing the player's car.
     */
    Snapshot::CarSnapshot buildCarSnapshot();
};

#endif
