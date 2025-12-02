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

#define TARGET_FPS 60
#define FRAME_DURATION_MS (1000 / TARGET_FPS)
#define TIME_STEP (1.0f / TARGET_FPS)  // Duration of each step that box2d simulates each frame

#define PIXELS_TO_METERS 0.01f  // 1 pixel = 0.01 meters (1 meter = 100 pixels)
#define WORLD_HEIGHT 4672.0f  // In pixels
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
    std::vector<std::string> allRaceFiles;
    #ifdef INSTALL_MODE
        std::string directory = "/etc/needForSpeed2D/server/gameLogic/races/";
    #else
        std::string directory = "server/gameLogic/races/";
    #endif

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
        throw std::runtime_error("Not enough race files to choose " +
                                 std::to_string(races) + " races.");
    }

    raceFiles.assign(allRaceFiles.begin(), allRaceFiles.begin() + races);
}

b2Body* Game::createNewCarBody() {
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position.Set(WORLD_HEIGHT_METERS / 2, WORLD_HEIGHT_METERS / 2);  // Almost the center
    body_def.angle = 1.571f;  // 90 degrees in radians

    b2Body* car = world->CreateBody(&body_def);

    return car;
}

void Game::updatePlayerCars() {
    for (auto& [id, player]: players) {
        player.updateNPCDirections();
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
    // Countdown is sent?
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
        case GameState::RACING:
            broadcastRacing();
            break;
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
    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>());
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
    return std::chrono::seconds(0);  // To avoid compiler warnings
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
    for(b2Body* body : NPCGraphBodies) {
        world->DestroyBody(body);
    }

    pathBodies.clear();
    lowCollisionLayerBodies.clear();
    highCollisionLayerBodies.clear();
    layerSwitchBodies.clear();
    NPCGraphBodies.clear();
}

void Game::setCountdownState() {
    setGameState(GameState::COUNTDOWN);

    clearMapBodies();

    Path currentPath = PathLoader::LoadPath(raceFiles[currentRaceCount++]);
    pathBodies = PathGenerator::GeneratePath(currentPath, world, PIXELS_TO_METERS, WORLD_HEIGHT);

    std::string mapName;
    Graph currentNpcPaths;
    switch (currentPath.mapId){
        case MAP_LIBERTY_CITY:
            mapName = "libertyCity";
            break;
        case MAP_SAN_ANDREAS:
            mapName = "sanAndreas";
            break;
        case MAP_VICE_CITY:
            mapName = "viceCity";
            #ifdef INSTALL_MODE
                currentNpcPaths = GraphLoader::LoadGraph("/etc/needForSpeed2D/server/gameLogic/npcPath.yaml", PIXELS_TO_METERS, WORLD_HEIGHT);
            #else
                currentNpcPaths = GraphLoader::LoadGraph("server/gameLogic/npcPath.yaml", PIXELS_TO_METERS, WORLD_HEIGHT);
            #endif

            players.emplace(std::piecewise_construct,
                        std::forward_as_tuple(MAX_PLAYERS),
                        std::forward_as_tuple(MAX_PLAYERS, "NPC", config.carsInfo, 0, createNewCarBody()));
            break;
    }
    
    // If graph is not empty, give it to the NPCs
    for (auto& [id, player] : players) {
        if(id < MAX_PLAYERS)
            player.initCurrentPath(currentPath);
        else if (currentNpcPaths.mapId != -1)
            player.initCurrentGraph(currentNpcPaths);
    }

    // Uncomment to play as the NPC (and follow it with the camera)
    // for (auto& [id, player] : players) {
    //     if (currentNpcPaths.mapId != -1)
    //         player.initCurrentGraph(currentNpcPaths);
    // }

    #ifdef INSTALL_MODE
        CollisionMap lowLayerCollisionMap = CollisionLoader::LoadCollisions("/etc/needForSpeed2D/server/gameLogic/collisions/maps/" + mapName + "/low_collision_layer.yaml");
        CollisionMap highLayerCollisionMap = CollisionLoader::LoadCollisions("/etc/needForSpeed2D/server/gameLogic/collisions/maps/" + mapName + "/high_collision_layer.yaml");
        CollisionMap layerSwitchCollisionMap = CollisionLoader::LoadCollisions("/etc/needForSpeed2D/server/gameLogic/collisions/maps/" + mapName + "/layer_switch.yaml");
    #else
        CollisionMap lowLayerCollisionMap = CollisionLoader::LoadCollisions("server/gameLogic/collisions/maps/" + mapName + "/low_collision_layer.yaml");
        CollisionMap highLayerCollisionMap = CollisionLoader::LoadCollisions("server/gameLogic/collisions/maps/" + mapName + "/high_collision_layer.yaml");
        CollisionMap layerSwitchCollisionMap = CollisionLoader::LoadCollisions("server/gameLogic/collisions/maps/" + mapName + "/layer_switch.yaml");
    #endif
    

    lowCollisionLayerBodies = CollisionGenerator::GenerateCollisions(lowLayerCollisionMap, world, PIXELS_TO_METERS, WORLD_HEIGHT, WALL_LOW_LAYER);
    highCollisionLayerBodies = CollisionGenerator::GenerateCollisions(highLayerCollisionMap, world, PIXELS_TO_METERS, WORLD_HEIGHT, WALL_HIGH_LAYER);
    layerSwitchBodies = CollisionGenerator::GenerateCollisions(layerSwitchCollisionMap, world, PIXELS_TO_METERS, WORLD_HEIGHT, SENSOR_LAYER, LAYER_SWITCH_SENSOR);

    // Prepares the players for the race (apply penalties, set health = max_health, etc)
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

// Changes the state and sends a race start broadcast
void Game::setRacingState() {
    setGameState(GameState::RACING);

    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(MSG_RACE_START));
}

void Game::setShowingStatsState() {
    // To the players who did not finish the race, assign a maximum arrival time
    for (auto& [id, player]: players) {
        if (!player.hasFinished()) {
            player.setArrivalTime(raceDuration.count());
        }
    }

    setGameState(GameState::SHOWING_STATS);
    // Race stats broadcast
    Snapshot::RaceResults results;
    for (auto& [id, player]: players) {
        if (id >= MAX_PLAYERS) continue;
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

    // To the players who did not finish the race, assign a maximum arrival time
    for (auto& [id, player]: players) {
        if (!player.hasFinished()) {
            player.setArrivalTime(raceDuration.count());
        }
    }
    
    // Match stats broadcast
    Snapshot::FinalResults results;
    for (auto& [id, player]: players) {
        if (id >= MAX_PLAYERS) continue;
        Snapshot::FinalResults::FinalStanding fs;
        fs.playerId = player.getClientId();
        fs.playerName = player.getUsername();
        fs.totalTimeMs = player.getTotalRaceTime();
        fs.position = 0;  // Assigned in the sort

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

    if (!results.standings.empty()) {  // Just in case
        results.winnerId   = results.standings[0].playerId;
        results.winnerName = results.standings[0].playerName;
    }
    
    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(results));
}

void Game::setModifyingCarState() {
    setGameState(GameState::MODIFYING_CAR);

    std::vector<Snapshot::CarProperties> carProps;
    for (auto& [id, player]: players) {
        if (id >= MAX_PLAYERS) continue;
        Snapshot::CarProperties prop = player.buildModifyingCarSnapshot();
        carProps.push_back(prop);
    }

    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(carProps));
}

// Just changes the current game state and updates the starting time point
void Game::setGameState(GameState newState) {
    currentState = newState;
    gameStateStartTime = std::chrono::high_resolution_clock::now();
}

bool Game::allPlayersFinished() {
    for (auto& [id, player] : players) {
        if (!player.hasFinished() && id < MAX_PLAYERS) return false;
    }
    return true;
}

void Game::updateGameState() {
    auto now = std::chrono::high_resolution_clock::now();
    auto gameStateElapsed = now - gameStateStartTime;

    switch (currentState) {
        case GameState::COUNTDOWN:
            if (gameStateElapsed >= countdownDuration) {
                setRacingState();
            }
            break;
        case GameState::RACING:
            if (gameStateElapsed >= raceDuration || allPlayersFinished()) {
                if (currentRaceCount >= races) {
                    setGameEndState();
                } else {
                    setShowingStatsState();
                }
            }
            break;
        case GameState::SHOWING_STATS:
            if (gameStateElapsed >= statsDuration) {
                setModifyingCarState();
            }
            break;
        case GameState::MODIFYING_CAR:
            if (gameStateElapsed >= upgradesDuration) {
                setCountdownState();
            }
            break;
        case GameState::GAME_END:
            // ?
            break;
        case GameState::ELIMINATED:
            // GameState::ELIMINATED is only useful for the client?
            // If a user dies, the server will keep sending racing snapshots
            // But it also sends the car's health
            // If the client checks that its health is 0, it switches to eliminated instead of racing
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
    // A snapshot is sent each gameloop
    // It is handled by broadcast, this function does nothing for now
}

void Game::handleRacingState() {
    // Command Pattern!
    std::unique_ptr<Command> cmd;
    while (clientCommandsQueue.tryPop(cmd)) {
        cmd->execute(*this);
    }

    updatePlayerCars();

    world->Step(TIME_STEP, velocityIt, positionIt);
}

void Game::handleShowingStatsState() {
    // For now, nothing
}

void Game::handleGameEndState() {
    // Same
}

void Game::handleModifyingCarState() {
    // Command Pattern again!
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

    Snapshot::CollisionData collisionData = player->buildCollisionSnapshot(impact);
    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(collisionData));
    
    // se deberia escuchar el choque contra autos destruidos, pero no se tendria que aplicar logica
    if(!player->isAlive()) return;
    
    player->applyCollisionDamage(impact);

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

    // ContactListener to handle collisions
    // A pointer to Game is passed so it can call Game::handleCollision
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
        // players.emplace(clientId, Player(clientId, username, newCarBody, carId));
        // Inside Player data->player = this is made (which points to the temporary), when data->player->applyDamage is called health takes garbage values
        // Construct player in-place to avoid the previous problem
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
    players.at(clientId).toggleImmortality();
}

void Game::makeInstaWin(ClientID clientId) {
    auto now = std::chrono::high_resolution_clock::now();
    auto gameStateElapsed = now - gameStateStartTime;
    std::chrono::seconds raceTimeSecs = std::chrono::duration_cast<std::chrono::seconds>(gameStateElapsed);
    players.at(clientId).instaWin(raceTimeSecs);
}

void Game::makeInstaLose(ClientID clientId) {
    handleCollision(&players.at(clientId), 999);
}

void Game::makePlayerGoSuperFast(ClientID clientId) {
    players.at(clientId).toggleSuperSpeed();
}

void Game::improveCarProperties(ClientID clientId, bool improveVelocity, bool improveHealth, bool improveAcceleration, bool improveMass) {
    auto& player = players.at(clientId);
    player.improveCarProperties(improveVelocity, improveHealth, improveAcceleration, improveMass);
}

void Game::disconnectPlayer(ClientID clientId) {
    (void)clientId;
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
