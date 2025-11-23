#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>
#include <utility>
#include <chrono>

#include "../../common/messages/snapshot.h"
#include "car.h"

class Player {
private:
    ClientID clientId;
    std::string username;
    Car car;

    bool finished;
    uint32_t currentRaceTime;
    uint32_t totalRaceTime;

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

    bool hasFinished();
    void setArrivalTime(float arrivalTime);

    std::string getUsername() const { return username; }
    uint32 getCurrentRaceTime() const { return currentRaceTime; }
    uint32 getTotalRaceTime() const { return totalRaceTime; }

    /**
     * Builds and returns a CarSnapshot representing the player's car.
     */
    Snapshot::CarSnapshot buildCarSnapshot();
};

#endif
