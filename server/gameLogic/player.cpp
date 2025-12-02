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
        clientId(clientId), username(username), car(CarBuilder::createCar(carsInfo, carId, body)), totalRaceTime(0), penalty(0), immortal(false), superSpeed(false), isNPC(false), currentNodeIndex(-1), targetNodeIndex(-1)
{
    auto* data = new BodyData(this);
    
    body->GetUserData().pointer = reinterpret_cast<uintptr_t>(data);

    if (clientId >= 8) {
        car.setMaxSpeed(car.getMaxSpeed()/5);
    }
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
            break;
        }
    }
}

void Player::initCurrentPath(Path& currentPath) {
    this->currentPath = currentPath;
    updateNextCheckpoint();
    car.setPosition(currentPath.carSpawns[clientId]);
}

// recibe el checkpoint que el jugador tocó
void Player::updateCurrentPath(PathElement& element, std::chrono::seconds raceTimeSecs) {
    if (nextCheckpoint != element) {
        return;
    } 

    auto& elements = currentPath.elements;

    auto it = std::find(elements.begin(), elements.end(), nextCheckpoint);
    if (it == elements.end()) {
        // raro que llegue aca
        return;
    }

    elements.erase(elements.begin(), it + 1);

    if(!elements.empty()) {
        updateNextCheckpoint();
    }
    else {
        // el jugador terminó el recorrido!
        setArrivalTime(raceTimeSecs.count());
        nextCheckpoint = PathElement();
    }
}

void Player::initCurrentGraph(const Graph& g) {
    isNPC = true;
    npcNodes = g.nodes;
    npcEdges = g.edges;

    // arranca en el nodo 0 para que sea facil encontrar el NPC
    currentNodeIndex = 0;
    // si se quiere hacer random, descomentar la siguiente linea y comentar la anterior
    // currentNodeIndex = rand() % npcNodes.size();

    // no hay un nodo previo asi que se hardcodea un id muy grande
    targetNodeIndex = chooseNextNode(currentNodeIndex, 999);

    GraphNode& start = npcNodes[currentNodeIndex];

    // por el momento el angulo no importa
    // si no tiene target node entonces el auto se va a quedar quieto, no importa su angulo
    car.spawnAsNPC(start.x, start.y, 0);

    if (targetNodeIndex >= 0) {
        GraphNode& next = npcNodes[targetNodeIndex];
        float dx = next.x - start.x;
        float dy = next.y - start.y;
        float angle = std::atan2(dy,dx);
        car.spawnAsNPC(start.x, start.y, angle);
    }
}

int Player::chooseNextNode(int currentNode, int prevNode) {
    std::vector<int> neighbors;
    for (auto& e : npcEdges) {
        if (e.first == currentNode && e.second != prevNode) neighbors.push_back(e.second);
        else if (e.second == currentNode && e.first != prevNode) neighbors.push_back(e.first);
    }
    
    // si un NPC llega a una calle sin salida que se quede quieto
    // (es dificil lograr que salga por donde vino, porque el giro es lento y las calles son angostas)
    if (neighbors.empty())
        return currentNode;

    int nextNode = neighbors[rand() % neighbors.size()];
    return nextNode;
}

void Player::updateNPCDirections() {
    if (!isNPC) return;

    // debugPrintCarInfo();

    npcControls = ActiveDirections();

    // vector hacia el nodo target
    auto& target = npcNodes[targetNodeIndex];

    b2Vec2 pos = car.getPosition();
    float dx = target.x - pos.x;
    float dy = target.y - pos.y;

    float dist = sqrtf(dx*dx + dy*dy);

    // si llegó, elegir otro
    if (dist < 0.1f) {
        int prev = currentNodeIndex;
        currentNodeIndex = targetNodeIndex;
        targetNodeIndex = chooseNextNode(currentNodeIndex, prev);
        return;
    }

    // calcular ángulo deseado
    float desiredAngle = atan2(dy, dx);
    float currentAngle = car.getAngle();

    // normalizar el angulo para que los angulos de giro sean los adecuados
    // (garantiza que el angulo esta entre -pi y +pi)
    // sin esto el NPC puede entrar en un loop infinito de giro
    float angleDiff = desiredAngle - currentAngle;
    while (angleDiff >  M_PI) angleDiff -= 2*M_PI;
    while (angleDiff < -M_PI) angleDiff += 2*M_PI;

    // lo unico que cambia es el giro, porque va a acelerar todo el rato
    // le doy un umbral para que el giro sea relativamente suave (que no "snapee" al nodo como un robot)
    if (angleDiff < -0.05f) 
        npcControls.right = true;
    else if (angleDiff > 0.05f) 
        npcControls.left = true;
    
    npcControls.up = true;

    car.updateActiveDirections(npcControls);
}


void Player::handleDeath(std::chrono::seconds raceDurationSecs) {
    setArrivalTime(raceDurationSecs.count());
}

void Player::debugPrintCarInfo() {
    // system("clear");
    BodyData* data = reinterpret_cast<BodyData*>(car.getBody()->GetUserData().pointer);
    std::cout << "[DEBUG] Car - Player: " << data->player->getUsername() << std::endl
            //   << " | CarID  : " << car.getId() << std::endl
              << " | Position: (" << car.getPosition().x << ", " << car.getPosition().y << ")" << std::endl
              << " | Angle  : " << car.getAngle() << std::endl
            //   << " | MaxSpeed   : " << car.getMaxSpeed() << std::endl
              << " | Speed      : " << car.getCurrentSpeed() << std::endl
            //   << " | Health : " << car.getCurrentHealth() << std::endl
            //   << " | Accelerat. : " << car.getAcceleration() << std::endl
            //   << " | Mass       : " << car.getMass() << std::endl
            //   << " | OnBridge: " << car.isOnBridge() << std::endl
            ;
}

Snapshot::CarSnapshot Player::buildCarSnapshot() {
    // debugPrintCarInfo();
    uint32 x = static_cast<uint32_t>(std::round(car.getPosition().x / PIXELS_TO_METERS * 1000));
    uint32 y = static_cast<uint32_t>(std::round(car.getPosition().y / PIXELS_TO_METERS * 1000));

    // normalizar el angulo para que este entre 0 y 360
    // evita el "snap" visual de cuando hay un overflow en el angulo
    float angleDeg = -car.getAngle() * RADTODEG;
    angleDeg = fmodf(angleDeg, 360.0f);
    if (angleDeg < 0.0f)
        angleDeg += 360.0f;
    uint16 angle = static_cast<uint16_t>(std::round(angleDeg));

    uint16_t speed = static_cast<uint16_t>(std::round(car.getCurrentSpeed() * 1000));

    uint8_t healthPercentage = static_cast<uint8_t>(std::round((car.getCurrentHealth() / car.getMaxHealth()) * 100));
    
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
