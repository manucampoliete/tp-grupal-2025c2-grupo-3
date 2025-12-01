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

#include "collisions/pathLoader.h"

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

    Path currentPath;
    PathElement nextCheckpoint;

    bool immortal;
    bool superSpeed;

    void debugPrintCarInfo();

    void updateNextCheckpoint();

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
    void applyCollisionDamage(float impact) { if(!immortal) car.applyDamage(impact); }

    void initCurrentPath(Path& currentPath);
    void updateCurrentPath(PathElement& element, std::chrono::seconds raceTimeSecs);

    // aplica finished = true y le asigna la maxima duracion posible de la carrera como tiempo de carrera
    void handleDeath(std::chrono::seconds raceDurationSecs);

    /**
     * Builds and returns a CarSnapshot representing the player's car.
     */
    Snapshot::CarSnapshot buildCarSnapshot();
    Snapshot::CarProperties buildModifyingCarSnapshot();

    Snapshot::CollisionData buildCollisionSnapshot(float impact);

    ClientID getClientId() const { return clientId; }
    bool isAlive() { return car.getCurrentHealth() > 0.0f; }

    void toggleImmortality() { immortal = !immortal; }
    void instaWin(std::chrono::seconds raceTimeSecs);
    void toggleSuperSpeed();

    Car getCar() const { return car; }

    ~Player();
};

#endif
