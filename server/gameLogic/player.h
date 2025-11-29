#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>
#include <utility>
#include <chrono>
#include <iostream>

#include "../../common/messages/snapshot.h"
#include "../configLoader.h"
#include "car.h"

#define PENALTY_PER_IMPROVEMENT 5

class Player {
private:
    ClientID clientId;
    std::string username;
    Car car;

    bool finished;
    uint32_t currentRaceTime;
    uint32_t totalRaceTime;

    uint32_t penalty;

    void debugPrintCarInfo();

public:
    /**
     * Constructor
     */
    Player(ClientID clientId, const std::string& username, const std::vector<CarInfo>& carsInfo, CarID carId, b2Body* body);

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
    uint16 getCarSpeed() const { return car.getCurrentSpeed(); }
    uint8 getCarHealth() const { return car.getMaxHealth(); }

    uint16 getCarAcceleration() const { return car.getAcceleration(); }
    uint16 getCarMass() const { return car.getMass(); }

    void improveCarProperties(bool improveVelocity, bool improveHealth, bool improveAcceleration, bool improveMass);

    void resetForNewRace();
    void applyCollisionDamage(float impact) { car.applyDamage(impact); }

    /**
     * Builds and returns a CarSnapshot representing the player's car.
     */
    Snapshot::CarSnapshot buildCarSnapshot();
    Snapshot::CarProperties buildModifyingCarSnapshot();

    Snapshot::CollisionData buildCollisionSnapshot(float impact);

    ClientID getClientId() const { return clientId; }
    bool isAlive() { return car.getCurrentHealth() > 0.0f; }

    Car getCar() const { return car; }

    ~Player();
};

#endif
