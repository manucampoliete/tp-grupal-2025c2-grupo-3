#include "game.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <utility>
#include <vector>
#include <tuple>

#include <iostream>

#include "collisions/collisionLoader.h"
#include "collisions/contactListener.h"
#include "collisions/collisionBits.h"
#include "../../common/constantRateLoop/constantRateLoop.h"
#include "../../common/messages/snapshot.h"

#define TARGET_FPS 60
#define FRAME_DURATION_MS (1000 / TARGET_FPS)
#define TIME_STEP (1.0f / TARGET_FPS) // duracion del step que simula box2d cada frame

#define PIXELS_TO_METERS 0.01f // 1 pixel = 0.01 metros (1 metro = 100 pixeles)
#define WORLD_HEIGHT 4672.0f // en pixeles
#define WORLD_HEIGHT_METERS (WORLD_HEIGHT * PIXELS_TO_METERS)


#define MAX_PLAYERS 8
#define RACE_DURATION 5

Game::Game():
        world(std::make_unique<b2World>(b2Vec2(0, 0))),
        velocityIt(8),
        positionIt(3),
        clientCommandsQueue(),
        responseQueuesMonitor(),
        players(),
        countdownDuration(3),
        raceDuration(10),
        statsDuration(5),
        upgradesDuration(5),
        started(false) {}

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
    /* filter.categoryBits = CAR_LOW_LAYER;
    filter.maskBits = MASK_CAR_LOW; */
    filter.categoryBits = CAR_HIGH_LAYER;
    filter.maskBits = MASK_CAR_HIGH;
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
        case GameState::ELIMINATED:
        case GameState::GAME_END:
            // implementar cuando haga falta
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

void Game::setCountdownState() {
    setGameState(GameState::COUNTDOWN);

    // incializar players (aplicar penalizaciones, poner vida = max_vida)
    // BUG: si un jugador finaliza la carrera por tiempo limite no se le aplica la penalizacion!
    // se arregla en showing stats? (sumarle el tiempo maximo al tiempo de carrera actual, que es donde se refleja la penalizacion)
    for (auto& [id, player]: players) {
        player.resetForNewRace();
    }

    // no se hace el primer broadcast para countdown, se manda solo el broadcast por frame
    // responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(MSG_COUNTDOWN_START));
}

// cambia el estado y manda el broadcast de que se empezó la carrera
void Game::setRacingState() {
    setGameState(GameState::RACING);

    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(MSG_RACE_START));
}

void Game::setShowingStatsState() {
    // a los jugadores que no terminaron la carrera se les asigna un tiempo de llegada maximo
    auto now = std::chrono::high_resolution_clock::now();
    auto gameStateElapsed = now - gameStateStartTime;
    std::chrono::seconds raceTimeSecs = std::chrono::duration_cast<std::chrono::seconds>(gameStateElapsed);
    
    for (auto& [id, player]: players) {
        if (!player.hasFinished()) {
            player.setArrivalTime(raceTimeSecs.count());
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

void Game::setModifyingCarState() {
    setGameState(GameState::MODIFYING_CAR);

    std::vector<Snapshot::CarProperties> carProps;
    for (auto& [id, player]: players) {
        Snapshot::CarProperties prop;
        prop.playerId = id;
        prop.speed = player.getCarSpeed();
        prop.health = player.getCarHealth();
        carProps.push_back(prop);
    }

    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(carProps));
}

// solo cambia al estado de juego dado
void Game::setGameState(GameState newState) {
    currentState = newState;
    gameStateStartTime = std::chrono::high_resolution_clock::now();
    std::cout << "Game state changed to " << static_cast<int>(newState) << std::endl;
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
            if (gameStateElapsed >= raceDuration) {
                setShowingStatsState();
            }
            break;
        case GameState::SHOWING_STATS:
            if (gameStateElapsed >= statsDuration) {
                setModifyingCarState();
            }
            break;
        case GameState::MODIFYING_CAR:
            if (gameStateElapsed >= upgradesDuration) {
                // aumentar numero de carrera en 1
                // pasar a countdown
                // setGameState(GameState::COUNTDOWN);
                setCountdownState();
            }
            break;
        case GameState::ELIMINATED: 
        // eliminated le sirve solo al cliente?
        // si un usuario muere el server le va a estar mandando snapshots de carrera
        // pero tambien manda la vida del auto
        // si el cliente checkea que su vida es 0, pasa a eliminated en vez de racing
        case GameState::GAME_END:
            // ?
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
        case GameState::ELIMINATED:
            // handleEliminatedState();
            break;
        case GameState::GAME_END:
            // handleGameEndState();
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
    // el handler de showing stats es el que tiene que checkear si
    // ya no quedan más carreras por correr 
    // (para mostrar un ganador y no mostrar la pantalla de mejoras)

    // por el momento implementar logica de carrera terminada por tiempo cumplido
    // cuando este la carrera hecha agregar la logica de tiempo de finalizacion
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
        responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(player->getClientId()));
    }
}

void Game::run() {
    started = true;
    
    broadcastStartSignal();
    setGameState(GameState::COUNTDOWN);

    // refactor (init collisions)
    CollisionLoader::LoadCollisions("server/gameLogic/collisions/low_collision_layer.yaml", world, PIXELS_TO_METERS, WORLD_HEIGHT, WALL_LOW_LAYER);

    CollisionLoader::LoadCollisions("server/gameLogic/collisions/high_collision_layer.yaml", world, PIXELS_TO_METERS, WORLD_HEIGHT, WALL_HIGH_LAYER);

    CollisionLoader::LoadCollisions("server/gameLogic/collisions/layer_switch.yaml", world, PIXELS_TO_METERS, WORLD_HEIGHT, SENSOR_LAYER, IS_SENSOR);
    
    // contact listener para manejar choques
    // se le pasa un puntero a Game para que pueda llamar a handleCollision
    ContactListener contactListener(this);
    world->SetContactListener(&contactListener);

    uint64_t lastIt = 0;
    uint64_t it = 0;
    ConstantRateLoop crl;
    while (shouldKeepRunning()) {
        // Do some stuff
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
    Thread::stop();
    clientCommandsQueue.close();  // Receivers cannot push, game cannot tryPop (once it's empty)
    responseQueuesMonitor.closeAll();  // Senders cannot pop, game cannot tryPush when broadcasting
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
                        std::forward_as_tuple(clientId, username, createNewCarBody(), carId));
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
}

void Game::makePlayerGoSuperFast(ClientID clientId) {
    std::cout << "Making player " << clientId << " go super fast!" << std::endl;
    /**
     * TODO: implement this method
     * NOTE: we can increase the player's car velocity temporarily (or permanently?)
     */
}

void Game::improveCarProperties(ClientID clientId, bool improveVelocity, bool improveHealth) {
    std::cout << "Improving car properties for player " << clientId << ": "
              << (improveVelocity ? "velocity " : "") << (improveHealth ? "health" : "") << std::endl;
    
    /**
     * TODO: implement this method
     * REMEMBER: Each improvement has a cost that is computed as a penalty to the arrival time
     * 
     * Could be something like:
     */

    auto& player = players.at(clientId);
    player.improveCarProperties(improveVelocity, improveHealth);
}

Game::~Game() {}
