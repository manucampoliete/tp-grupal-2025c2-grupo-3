#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>
#include <utility>

#include "../../common/messages/snapshot.h"
#include "car.h"

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

    void applyCollisionDamage(float impact) { car.applyDamage(impact); }

    /**
     * Builds and returns a CarSnapshot representing the player's car.
     */
    Snapshot::CarSnapshot buildCarSnapshot();

    Snapshot::CollisionData buildCollisionSnapshot(float impact);

    ClientID getClientId() const { return clientId; }
    bool isAlive() { return car.getHealth() > 0.0f; }

    Car getCar() const { return car; }
};

#endif
