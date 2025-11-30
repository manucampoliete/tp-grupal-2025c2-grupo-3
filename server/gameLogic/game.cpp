#include "game.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <utility>
#include <vector>
#include <tuple>
#include <iostream>
#include <random>
#include <filesystem>

#include "collisions/collisionLoader.h"
#include "collisions/collisionGenerator.h"
#include "collisions/contactListener.h"
#include "collisions/collisionBits.h"
#include "../../common/constantRateLoop/constantRateLoop.h"
#include "../../common/messages/snapshot.h"

#include "collisions/pathGenerator.h"

#define TARGET_FPS 60
#define FRAME_DURATION_MS (1000 / TARGET_FPS)
#define TIME_STEP (1.0f / TARGET_FPS) // duracion del step que simula box2d cada frame

#define PIXELS_TO_METERS 0.01f // 1 pixel = 0.01 metros (1 metro = 100 pixeles)
#define WORLD_HEIGHT 4672.0f // en pixeles
#define WORLD_HEIGHT_METERS (WORLD_HEIGHT * PIXELS_TO_METERS)


#define MAX_PLAYERS 8
#define RACE_DURATION 5

Game::Game(const Config& config):
        config(config),
        world(std::make_unique<b2World>(b2Vec2(0, 0))),
        velocityIt(8),
        positionIt(3),
        clientCommandsQueue(),
        responseQueuesMonitor(),
        players(),
        races(config.numberOfRaces),
        currentRaceCount(0),
        countdownDuration(config.gamePhasesTimers.countdown),  
        raceDuration(config.gamePhasesTimers.racing),       
        statsDuration(config.gamePhasesTimers.showingStats),      
        upgradesDuration(config.gamePhasesTimers.modifyingCar),   
        started(false)
{
    /* std::vector<std::string> allRaceFiles = {
        "map1.yaml",
        "map2.yaml",
        "map3.yaml"
    }; */

    std::vector<std::string> allRaceFiles;
    std::string directory = "server/gameLogic/races/";

    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (!entry.is_regular_file()) continue;

        if (entry.path().extension() == ".yaml") {
            allRaceFiles.push_back(entry.path().string());
        }
    }

    std::shuffle(
        allRaceFiles.begin(),
        allRaceFiles.end(),
        std::mt19937{std::random_device{}()}
    );

    if (races > allRaceFiles.size()) {
        throw std::runtime_error("No hay suficientes archivos carreras para elegir " +
                                 std::to_string(races) + " carreras.");
    }

    raceFiles.assign(allRaceFiles.begin(), allRaceFiles.begin() + races);
}

b2Body* Game::createNewCarBody() {
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    // body_def.position.Set(0, 0);
    body_def.position.Set(WORLD_HEIGHT_METERS / 2, WORLD_HEIGHT_METERS / 2); // casi el centro
    body_def.angle = 1.571f; // 90 grados en radianes

    // box2d permite darle el caracter de "bala" a objetos para que estos atraviesen colisiones lo menos posible
    // sin esto un auto muy rapido podria atravesar edificios
    // body_def.bullet = true;

    b2Body* car = world->CreateBody(&body_def);

    b2PolygonShape boxShape;
    float pixelWidth = 28.0f;
    float pixelHeight = 22.0f;
    boxShape.SetAsBox(pixelWidth/2 * PIXELS_TO_METERS, pixelHeight/2 * PIXELS_TO_METERS);  // SetAsBox recibe "half-width" y "half-height"

    b2FixtureDef boxFixtureDef;
    boxFixtureDef.shape = &boxShape;
    boxFixtureDef.density = 1;
    // boxFixtureDef.friction = 0.3f;
    boxFixtureDef.restitution = 0.0f;  // poco rebote

    // capa de colision
    b2Filter filter;
    filter.categoryBits = CAR_LOW_LAYER;
    filter.maskBits = MASK_CAR_LOW;
    /* filter.categoryBits = CAR_HIGH_LAYER;
    filter.maskBits = MASK_CAR_HIGH; */
    boxFixtureDef.filter = filter;

    car->CreateFixture(&boxFixtureDef);

    // car->SetLinearDamping(0.5f);  // para que se frene con el tiempo

    return car;
}

void Game::updatePlayerCars() {
    for (auto& [id, player]: players) {
        player.updateCarPhysics();
    }
}

void Game::broadcastCountdown() {
    auto remaining = getRemainingGameStateTime();
    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(static_cast<uint32_t>(remaining.count()), MSG_COUNTDOWN));
}

void Game::broadcastRacing() {
    auto remaining = getRemainingGameStateTime();
    std::vector<Snapshot::CarSnapshot> snapshots;
    for (auto& [clientId, player]: players) {
        Snapshot::CarSnapshot snp = player.buildCarSnapshot();
        snapshots.emplace_back(snp);
    }

    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(static_cast<uint32_t>(remaining.count()), snapshots));
}

void Game::broadcastShowingStats() {
    auto remaining = getRemainingGameStateTime();
    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(static_cast<uint32_t>(remaining.count()), MSG_STATS_COUNTDOWN));
}

void Game::broadcastGameEnd() {
    // se manda un countdown?
}

void Game::broadcastModifyingCar() {
    auto remaining = getRemainingGameStateTime();
    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(static_cast<uint32_t>(remaining.count()), MSG_MOD_COUNTDOWN));
}

void Game::broadcast() {
    switch (currentState) {
        case GameState::COUNTDOWN:
            broadcastCountdown();
            break;
        case GameState::RACING: {
            broadcastRacing();
            break;
        }
        case GameState::SHOWING_STATS:
            broadcastShowingStats();
            break;
        case GameState::MODIFYING_CAR:
            broadcastModifyingCar();
            break;
        case GameState::GAME_END:
            broadcastGameEnd();
            break;
        case GameState::ELIMINATED:
            break;
    }
}

void Game::broadcastStartSignal() {
    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>());  // dummy timestamp for now
}

std::chrono::seconds Game::getRemainingGameStateTime() {
    auto now = std::chrono::high_resolution_clock::now();
    auto gameStateElapsed = now - gameStateStartTime;

    switch (currentState) {
        case GameState::COUNTDOWN:
            return std::chrono::duration_cast<std::chrono::seconds>(
                    countdownDuration - gameStateElapsed);
        case GameState::RACING:
            return std::chrono::duration_cast<std::chrono::seconds>(
                    raceDuration - gameStateElapsed);
        case GameState::SHOWING_STATS:
            return std::chrono::duration_cast<std::chrono::seconds>(
                    statsDuration - gameStateElapsed);
        case GameState::MODIFYING_CAR:
            return std::chrono::duration_cast<std::chrono::seconds>(
                    upgradesDuration - gameStateElapsed);
        case GameState::ELIMINATED:
        case GameState::GAME_END:
            return std::chrono::seconds(0);
    }
    return std::chrono::seconds(0);  // para evitar warning
}

void Game::clearMapBodies() {
    for (b2Body* body : pathBodies) {
        world->DestroyBody(body);
    }
    for (b2Body* body : lowCollisionLayerBodies) {
        world->DestroyBody(body);
    }
    for (b2Body* body : highCollisionLayerBodies) {
        world->DestroyBody(body);
    }
    for (b2Body* body : layerSwitchBodies) {
        world->DestroyBody(body);
    }

    pathBodies.clear();
    lowCollisionLayerBodies.clear();
    highCollisionLayerBodies.clear();
    layerSwitchBodies.clear();
}

void Game::setCountdownState() {
    setGameState(GameState::COUNTDOWN);

    clearMapBodies();

    // Path currentPath = PathLoader::LoadPath("server/gameLogic/race.yaml");
    Path currentPath = PathLoader::LoadPath(raceFiles[currentRaceCount++]);
    pathBodies = PathGenerator::GeneratePath(currentPath, world, PIXELS_TO_METERS, WORLD_HEIGHT);

    for (auto& [id, player] : players) {
        player.initCurrentPath(currentPath);
    }

    std::string mapName;
    std::cout << "[GAME] Setting map to " << mapName << std::endl;
    switch (currentPath.mapId){
        case MAP_LIBERTY_CITY:
            mapName = "libertyCity";
            break;
        case MAP_SAN_ANDREAS:
            mapName = "sanAndreas";
            break;
        case MAP_VICE_CITY:
            mapName = "viceCity";
            break;
    }

    CollisionMap lowLayerCollisionMap = CollisionLoader::LoadCollisions("server/gameLogic/collisions/maps/" + mapName + "/low_collision_layer.yaml");
    CollisionMap highLayerCollisionMap = CollisionLoader::LoadCollisions("server/gameLogic/collisions/maps/" + mapName + "/high_collision_layer.yaml");
    CollisionMap layerSwitchCollisionMap = CollisionLoader::LoadCollisions("server/gameLogic/collisions/maps/" + mapName + "/layer_switch.yaml");

    lowCollisionLayerBodies = CollisionGenerator::GenerateCollisions(lowLayerCollisionMap, world, PIXELS_TO_METERS, WORLD_HEIGHT, WALL_LOW_LAYER);
    highCollisionLayerBodies = CollisionGenerator::GenerateCollisions(highLayerCollisionMap, world, PIXELS_TO_METERS, WORLD_HEIGHT, WALL_HIGH_LAYER);
    layerSwitchBodies = CollisionGenerator::GenerateCollisions(layerSwitchCollisionMap, world, PIXELS_TO_METERS, WORLD_HEIGHT, SENSOR_LAYER, LAYER_SWITCH_SENSOR);

    // pepara los players para la carrera (aplicar penalizaciones, poner vida = max_vida, etc)
    for (auto& [id, player]: players) {
        player.resetForNewRace();
    }

    world->ClearForces();

    Snapshot::RaceInfo info;
    info.mapId = currentPath.mapId;
    info.race = currentRaceCount;
    info.totalRaces = races; 

    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(info));
}

// cambia el estado y manda el broadcast de que se empezó la carrera
void Game::setRacingState() {
    setGameState(GameState::RACING);

    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(MSG_RACE_START));
}

void Game::setShowingStatsState() {
    // auto now = std::chrono::high_resolution_clock::now();
    // auto gameStateElapsed = now - gameStateStartTime;
    // std::chrono::seconds raceTimeSecs = std::chrono::duration_cast<std::chrono::seconds>(gameStateElapsed);

    // a los jugadores que no terminaron la carrera se les asigna un tiempo de llegada maximo
    for (auto& [id, player]: players) {
        if (!player.hasFinished()) {
            player.setArrivalTime(raceDuration.count());
        }
    }

    setGameState(GameState::SHOWING_STATS);
    // broadcast de estadisticas de carrera
    Snapshot::RaceResults results;
    for (auto& [id, player]: players) {
        Snapshot::RaceResults::PlayerResult pr;
        pr.playerName = player.getUsername();
        pr.raceTimeMs = player.getCurrentRaceTime();
        pr.totalTimeMs = player.getTotalRaceTime();
        results.players.push_back(pr);
    }

    std::sort(results.players.begin(), results.players.end(),
              [](const Snapshot::RaceResults::PlayerResult& a, const Snapshot::RaceResults::PlayerResult& b) {
                  return a.raceTimeMs < b.raceTimeMs;
              });
    
    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(results));
    
}

void Game::setGameEndState() {
    setGameState(GameState::GAME_END);

    // a los jugadores que no terminaron la carrera se les asigna un tiempo de llegada maximo
    for (auto& [id, player]: players) {
        if (!player.hasFinished()) {
            player.setArrivalTime(raceDuration.count());
        }
    }
    // broadcast de estadisticas de la partida
    Snapshot::FinalResults results;
    for (auto& [id, player]: players) {
        Snapshot::FinalResults::FinalStanding fs;
        fs.playerId = player.getClientId();
        fs.playerName = player.getUsername();
        fs.totalTimeMs = player.getTotalRaceTime();
        fs.position = 0; // se la doy en el sort

        results.standings.push_back(fs);
    }

    std::sort(results.standings.begin(), results.standings.end(),
              [](const Snapshot::FinalResults::FinalStanding& a,
                 const Snapshot::FinalResults::FinalStanding& b) {
                  return a.totalTimeMs < b.totalTimeMs;
              });

    for (size_t i = 0; i < results.standings.size(); ++i) {
        results.standings[i].position = static_cast<uint8_t>(i + 1);
    }

    if (!results.standings.empty()) {    // por las dudas
        results.winnerId   = results.standings[0].playerId;
        results.winnerName = results.standings[0].playerName;
    }
    
    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(results));
}

void Game::setModifyingCarState() {
    setGameState(GameState::MODIFYING_CAR);

    std::vector<Snapshot::CarProperties> carProps;
    for (auto& [id, player]: players) {
        /* Snapshot::CarProperties prop;
        prop.playerId = id;
        prop.speed = player.getCarSpeed();
        prop.health = player.getCarHealth();
        prop.acceleration = player.getCarAcceleration();
        prop.mass = player.getCarMass();
        carProps.push_back(prop); */
        Snapshot::CarProperties prop = player.buildModifyingCarSnapshot();
        carProps.push_back(prop);
    }

    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(carProps));
}

// solo cambia al estado de juego dado
void Game::setGameState(GameState newState) {
    currentState = newState;
    gameStateStartTime = std::chrono::high_resolution_clock::now();
    // std::cout << "Game state changed to " << static_cast<int>(newState) << std::endl;
}

bool Game::allPlayersFinished() {
    for (auto& [id, player] : players) {
        if (!player.hasFinished()) return false;
    }
    return true;
}

void Game::updateGameState() {
    auto now = std::chrono::high_resolution_clock::now();
    auto gameStateElapsed = now - gameStateStartTime;

    switch (currentState) {
        case GameState::COUNTDOWN:
            if (gameStateElapsed >= countdownDuration) {
                std::cout << "[GAME] Switched to racing state" << std::endl;
                setRacingState();
            }
            break;
        case GameState::RACING:
            if (gameStateElapsed >= raceDuration || allPlayersFinished()) {
                if (currentRaceCount >= races) {
                    std::cout << "[GAME] Switched to game end state" << std::endl;
                    setGameEndState();
                } else {
                    std::cout << "[GAME] Switched to showing stats state" << std::endl;
                    setShowingStatsState();
                }
            }
            break;
        case GameState::SHOWING_STATS:
            if (gameStateElapsed >= statsDuration) {
                std::cout << "[GAME] Switched to modifying car state" << std::endl;
                setModifyingCarState();
            }
            break;
        case GameState::MODIFYING_CAR:
            if (gameStateElapsed >= upgradesDuration) {
                std::cout << "[GAME] Switched to countdown state" << std::endl;
                setCountdownState();
            }
            break;
        case GameState::GAME_END:
            // ?
            break;
        case GameState::ELIMINATED: 
            // eliminated le sirve solo al cliente?
            // si un usuario muere el server le va a estar mandando snapshots de carrera
            // pero tambien manda la vida del auto
            // si el cliente checkea que su vida es 0, pasa a eliminated en vez de racing
            break;
    }
}

void Game::handleGameState() {
    switch (currentState) {
        case GameState::COUNTDOWN:
            handleCountdownState();
            break;
        case GameState::RACING:
            handleRacingState();
            break;
        case GameState::SHOWING_STATS:
            handleShowingStatsState();
            break;
        case GameState::MODIFYING_CAR:
            handleModifyingCarState();
            break;
        case GameState::GAME_END:
            handleGameEndState();
            break;
        case GameState::ELIMINATED:
            // handleEliminatedState();
            break;
    }
}

void Game::handleCountdownState() {
    // se manda un snapshot por gameloop
    // se encarga broadcast, esta funcion no hace nada por ahora
}

void Game::handleRacingState() {
    // Command Pattern
    std::unique_ptr<Command> cmd;
    while (clientCommandsQueue.tryPop(cmd)) {
        cmd->execute(*this);
    }

    updatePlayerCars();

    world->Step(TIME_STEP, velocityIt, positionIt);
}

void Game::handleShowingStatsState() {
    // por el momento nada
}

void Game::handleGameEndState() {
    // lo mismo
}

void Game::handleModifyingCarState() {
    std::unique_ptr<Command> cmd;
    while (clientCommandsQueue.tryPop(cmd)) {
        cmd->execute(*this);
    }
}

// Posible problema: si se hace broadcast de los 2 choques entonces el cliente va a reproducir el sonido del choque 2 veces!!
// Checkear si la posición del choque es la misma para ambos players (o muy cercana) asi el cliente sabe que es el mismo choque
void Game::handleCollision(Player* player, float impact) {
    if (!player) {
        std::cerr << "[GAME] handleCollision: player is null!" << std::endl;
        return;
    }

    const float IMPACT_DAMAGE_THRESHOLD = 0.2f; // umbral minimo para aplicar daño
    if (impact < IMPACT_DAMAGE_THRESHOLD) {
        return; // no aplicar daño ni notificar si el impacto es muy bajo
    }

    player->applyCollisionDamage(impact);
    Snapshot::CollisionData collisionData = player->buildCollisionSnapshot(impact);
    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(collisionData));

    if(!player->isAlive()) {
        std::chrono::seconds raceDurationSecs = std::chrono::duration_cast<std::chrono::seconds>(raceDuration);
        player->handleDeath(raceDurationSecs);
        responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(player->getClientId()));
    }
}

void Game::handleCheckpointContact(Player* player, PathElement& element) {
    auto now = std::chrono::high_resolution_clock::now();
    auto gameStateElapsed = now - gameStateStartTime;
    std::chrono::seconds raceTimeSecs = std::chrono::duration_cast<std::chrono::seconds>(gameStateElapsed);
    player->updateCurrentPath(element, raceTimeSecs);
}

void Game::run() {
    started = true;
    broadcastStartSignal();
    setCountdownState();

    // contact listener para manejar choques
    // se le pasa un puntero a Game para que pueda llamar a handleCollision
    ContactListener contactListener(this);
    world->SetContactListener(&contactListener);

    uint64_t lastIt = 0;
    uint64_t it = 0;
    ConstantRateLoop crl;
    while (shouldKeepRunning()) {
        updateGameState();
        uint64_t deltaIt = it - lastIt;
        while (deltaIt-- > 0)
            handleGameState();
        broadcast();
        lastIt = it;

        // CRL algorithm
        it = crl.sleepAndCalcIt();
    }
}

void Game::stop() {
    if (not isDead()) {
        Thread::stop();
        clientCommandsQueue.close();  // Receivers cannot push, game cannot tryPop (once it's empty)
    }
}

void Game::join() {
    if (started) Thread::join();
}

bool Game::isAlive() const {
    return !started or (started and Thread::isAlive());
}

bool Game::isDead() { return not isAlive(); }

Queue<std::unique_ptr<Command>>& Game::getClientCommandsQueue() { return clientCommandsQueue; }

Queue<std::shared_ptr<Snapshot>>& Game::getResponsesQueue(ClientID clientId) {
    return responseQueuesMonitor.getQueue(clientId);
}

bool Game::addPlayer(ClientID clientId, const std::string& username, CarID carId) {
    if (players.size() >= MAX_PLAYERS)
        return false;

    if (players.find(clientId) == players.end()) {
        // players.emplace(clientId, Player(clientId, username, newCarBody, carId)); //dentro de Player se hace data->player = this (que apunta al temporal), cuando se llama data->player->applyDamage health toma valores basura
        // construir player in-place para evitar el problema anterior
        players.emplace(std::piecewise_construct,
                        std::forward_as_tuple(clientId),
                        std::forward_as_tuple(clientId, username, config.carsInfo, carId, createNewCarBody()));
        responseQueuesMonitor.addQueue(clientId);
        return true;
    }
    return false;
}

void Game::movePlayer(ClientID clientId, ActiveDirections activeDirections) {
    players.at(clientId).move(activeDirections);
}

void Game::makeInmortal(ClientID clientId) {
    std::cout << "Making player " << clientId << " inmortal!" << std::endl;
    /**
     * TODO: implement this method
     * NOTE: we can set the player's car health to a very high value
     */
}

void Game::makeInstaWin(ClientID clientId) {
    std::cout << "Making player " << clientId << " insta win!" << std::endl;
    /**
     * TODO: implement this method
     * NOTE: we can set the player's car position to the finish line
     */
}

void Game::makeInstaLose(ClientID clientId) {
    std::cout << "Making player " << clientId << " insta lose!" << std::endl;
    /**
     * TODO: implement this method
     * NOTE: we can set the player's car health to 0
     */
    handleCollision(&players.at(clientId), 999);
}

void Game::makePlayerGoSuperFast(ClientID clientId) {
    std::cout << "Making player " << clientId << " go super fast!" << std::endl;
    /**
     * TODO: implement this method
     * NOTE: we can increase the player's car velocity temporarily (or permanently?)
     */
}

void Game::improveCarProperties(ClientID clientId, bool improveVelocity, bool improveHealth, bool improveAcceleration, bool improveMass) {
    // std::cout << "Improving car properties for player " << clientId << ": "
    //           << (improveVelocity ? "velocity " : "") << (improveHealth ? "health" : "")
    //           << (improveMass ? "mass" : "") << (improveAcceleration ? "acceleration" : "") << std::endl;

    auto& player = players.at(clientId);
    player.improveCarProperties(improveVelocity, improveHealth, improveAcceleration, improveMass);
}

void Game::disconnectPlayer(ClientID clientId) {
    std::cout << "Disconnecting player " << clientId << " from the game." << std::endl;

    // // Remove player from the game
    // auto it = players.find(clientId);
    // if (it != players.end()) {
    //     // Destroy the player's car body in the physics world
    //     b2Body* carBody = it->second.getCar().getBody();
    //     if (carBody) {
    //         world->DestroyBody(carBody);
    //     }

    //     // Remove player from the players map
    //     players.erase(it);
    // }

    // responseQueuesMonitor.removeQueue(clientId);
    // // Removing the queue would leave an invalid reference in the Sender
    // // Maybe ResponseQueuesMonitor should have a closeQueue method instead?
}

Game::~Game() {
    for (b2Body* body = world->GetBodyList(); body; body = body->GetNext()) {
        auto ptr = body->GetUserData().pointer;
        if (ptr != 0) {
            BodyData* data = reinterpret_cast<BodyData*>(ptr);
            delete data;  // Free memory
            body->GetUserData().pointer = 0;
        }
    }
    stop();
    join();
}
