#include "player.h"

#include "carBuilder.h"
#include "yaml-cpp/yaml.h"

#include "bodyData.h"

#include <iostream>

#include <cstdlib>

#include "../../common/utils/pathElements.h"



#define RADTODEG 57.295779513082320876f
#define PIXELS_TO_METERS 0.01f // 1 pixel = 0.01 meters (1 meter = 100 pixels)

Player::Player(ClientID clientId, const std::string& username, const std::vector<CarInfo>& carsInfo, CarID carId, b2Body* body):
        clientId(clientId), username(username), car(CarBuilder::createCar(carsInfo, carId, body)), totalRaceTime(0), penalty(0), immortal(false), superSpeed(false)
{
    auto* data = new BodyData(this);
    
    // It seems this is the modern way to do it
    // SetUserData is from old versions of box2d
    body->GetUserData().pointer = reinterpret_cast<uintptr_t>(data);
}

void Player::move(ActiveDirections activeDirections) {
    car.updateActiveDirections(activeDirections);
}

void Player::updateCarPhysics() { car.updatePhysics(finished); }

bool Player::hasFinished() { return finished; }

void Player::setArrivalTime(float arrivalTime) {
    if (!finished) {
        finished = true;
        currentRaceTime = static_cast<uint32_t>(std::round(arrivalTime * 1000));\
        // Sum penalty if it exists
        currentRaceTime += (penalty > 0) ? (penalty * 1000) : 0;
        totalRaceTime += currentRaceTime;
        penalty = 0;
    }
}

void Player::updateNextCheckpoint() {
    for (auto& element : currentPath.elements) {
        bool isCheckpoint = (element.id == CHECKPOINT_HORIZONTAL ||
                            element.id == CHECKPOINT_VERTICAL ||
                            element.id == FINISH_HORIZONTAL ||
                            element.id == FINISH_VERTICAL);
        
        if(isCheckpoint) {
            nextCheckpoint = element;
            // std::cout << "[NEXT_CHK]: " << element << std::endl;
            break;
        }
    }
}

void Player::initCurrentPath(Path& currentPath) {
    this->currentPath = currentPath;
    updateNextCheckpoint();
    // ponerlos en el spawn
    // path tiene el vector std::vector<PathElement> carSpawns;
    // spawnear el player con clientId en carSpawns[cliendId];
    car.setPosition(currentPath.carSpawns[clientId]);
}

// recibe el checkpoint que el jugador tocó
void Player::updateCurrentPath(PathElement& element, std::chrono::seconds raceTimeSecs) {
    if (nextCheckpoint != element) {
        // std::cout << "[UPDATE_PATH] " << nextCheckpoint << " is not the same as incoming " << element << std::endl; 
        return;
    } 

    auto& elements = currentPath.elements;

    auto it = std::find(elements.begin(), elements.end(), nextCheckpoint);
    if (it == elements.end()) {
        // raro que llegue aca
        // std::cerr << "[UPDATE_PATH] Error! el nextCheckpoint no se encontró en el path actual";
        return;
    }

    elements.erase(elements.begin(), it + 1);

    if(!elements.empty()) {
        // std::cout << "[UPDATE_PATH] La carrera todavia tiene elementos:" << std::endl;
        /* for (auto& element : elements) {
            std::cout << element << std::endl;
        } */
        updateNextCheckpoint();
    }
    else {
        // el jugador terminó el recorrido!
        // std::cout << "[UPDATE_PATH] Player " << clientId << " terminó la carrera!" << std::endl;
        setArrivalTime(raceTimeSecs.count());
        nextCheckpoint = PathElement();
    }
    // std::cout <<"[UPDATE_PATH] New path size: " << currentPath.elements.size() << std::endl;
}

void Player::handleDeath(std::chrono::seconds raceDurationSecs) {
    setArrivalTime(raceDurationSecs.count());
}

/* void debugPrintCarInfo(ClientID clientId, uint32 x, uint32 y, uint16 angle, uint16 speed, CarID carId, uint8_t healthPercentage, bool onBridge) {
    system("clear");
    std::cout << "[DEBUG] CarSnapshot - PlayerID: " << clientId << std::endl
              << " | CarID: " << carId << std::endl
              << " | Position (px): (" << x / 1000 << ", " << y / 1000 << ")" << std::endl
              << " | Angle: " << angle << std::endl
              << " | Speed: " << speed / 1000 << std::endl
              << " | Health%: " << static_cast<int>(healthPercentage) << "%" << std::endl
              << " | OnBridge: " << (onBridge ? "Yes" : "No") << std::endl;
} */

void Player::debugPrintCarInfo() {
    // system("clear");
    BodyData* data = reinterpret_cast<BodyData*>(car.getBody()->GetUserData().pointer);
    std::cout << "[DEBUG] Car - Player: " << data->player->getUsername() << std::endl
            //   << " | CarID  : " << car.getId() << std::endl
            //   << " | Position: (" << car.getPosition().x << ", " << car.getPosition().y << ")" << std::endl
            //   << " | Angle  : " << car.getAngle() << std::endl
            //   << " | MaxSpeed : " << car.getMaxSpeed() << std::endl
            //   << " | Speed  : " << car.getCurrentSpeed() << std::endl
            //   << " | Health : " << car.getCurrentHealth() << std::endl
            //   << " | Acc    : " << car.getAcceleration() << std::endl
            //   << " | Mass   : " << car.getMass() << std::endl
              << " | OnBridge: " << car.isOnBridge() << std::endl
            ;
}

Snapshot::CarSnapshot Player::buildCarSnapshot() {
    // debugPrintCarInfo();
    uint32 x = static_cast<uint32_t>(std::round(car.getPosition().x / PIXELS_TO_METERS * 1000));
    uint32 y = static_cast<uint32_t>(std::round(car.getPosition().y / PIXELS_TO_METERS * 1000));

    // Normalization of the angle to be between 0 and 360
    // It avoids a "snap" in the animation when the angle overflows
    float angleDeg = -car.getAngle() * RADTODEG;
    angleDeg = fmodf(angleDeg, 360.0f);
    if (angleDeg < 0.0f)
        angleDeg += 360.0f;
    uint16 angle = static_cast<uint16_t>(std::round(angleDeg));

    uint16_t speed = static_cast<uint16_t>(std::round(car.getCurrentSpeed() * 1000));

    uint8_t healthPercentage = static_cast<uint8_t>(std::round((car.getCurrentHealth() / car.getMaxHealth()) * 100));

    // debugPrintCarInfo(clientId, x, y, angle, speed, car.getId(), healthPercentage, car.isOnBridge());
    
    // multiplicar por 1000 todas las coordenadas de los elementos del path y mandar (ya estan en pixeles y en coordenadas de sdl2)
    std::vector<PathElement> path;
    for (auto& element : currentPath.elements) {
        PathElement e;
        e.id = element.id;
        e.x = element.x * 1000;
        e.y = element.y * 1000;
        path.push_back(e);
    }

    return Snapshot::CarSnapshot(clientId, x, y, angle, speed, car.getId(), healthPercentage, car.isOnBridge(), path);
}

Snapshot::CarProperties Player::buildModifyingCarSnapshot() {
    Snapshot::CarProperties prop;
    prop.playerId = clientId;
    prop.speed = car.getMaxSpeed() * 1000;
    prop.health = car.getMaxHealth() * 100; 
    prop.acceleration = car.getAcceleration() * 1000;
    prop.mass = car.getMass() * 1000;

    return prop;
}

void Player::improveCarProperties(bool improveVelocity, bool improveHealth, bool improveAcceleration, bool improveMass)  {
    car.improveProperties(improveVelocity, improveHealth, improveAcceleration, improveMass);
    penalty = 0;
    penalty += (improveVelocity ? PENALTY_PER_IMPROVEMENT : 0) +
                (improveHealth ? PENALTY_PER_IMPROVEMENT : 0) +
                (improveAcceleration ? PENALTY_PER_IMPROVEMENT : 0) +
                (improveMass ? PENALTY_PER_IMPROVEMENT : 0);
}

void Player::resetForNewRace() {
    finished = false;
    currentRaceTime = 0;
    car.setCurrentHealth(car.getMaxHealth());
    if(car.isOnBridge()) car.toggleCollisionLayer();
    car.resetSpeeds();
}

// Expects a normalized impact
Snapshot::CollisionData Player::buildCollisionSnapshot(float normalizedImpact) {
    Snapshot::CollisionData collision;
    collision.playerId = clientId;

    collision.intensity = normalizedImpact;

    uint32 x = static_cast<uint32_t>(std::round(car.getPosition().x / PIXELS_TO_METERS * 1000));
    uint32 y = static_cast<uint32_t>(std::round(car.getPosition().y / PIXELS_TO_METERS* 1000));

    collision.x = x;
    collision.y = y;

    return collision;
}

void Player::instaWin(std::chrono::seconds raceTimeSecs) {
    currentPath.elements.clear();
    setArrivalTime(raceTimeSecs.count());
    nextCheckpoint = PathElement();
}

void Player::toggleSuperSpeed() {
    car.toggleSuperSpeed(superSpeed);
    superSpeed = !superSpeed;
}

Player::~Player() {
}
