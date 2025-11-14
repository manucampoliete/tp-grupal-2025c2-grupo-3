#include "game.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <utility>
#include <vector>

#include <iostream>

#include "collision_loader.h"

#define TARGET_FPS 60
#define FRAME_DURATION_MS (1000 / TARGET_FPS)
#define PLAYER_SPEED 200.0f  // píxeles por segundo
#define WORLD_HEIGHT 4672.0f

Game::Game():
        world(new b2World(b2Vec2(0, 0))),
        velocityIt(8),
        positionIt(3),
        clientCommandsQueue(),
        responseQueuesMonitor(),
        players(),
        countdownDuration(3),
        raceDuration(10),
        statsDuration(5),
        upgradesDuration(10) {}

b2Body* Game::createNewCarBody() {
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    // body_def.position.Set(0, 0);
    body_def.position.Set(WORLD_HEIGHT / 2, WORLD_HEIGHT / 2);
    body_def.angle = 0;

    // box2d permite darle el caracter de "bala" a objetos para que estos atraviesen colisiones lo menos posible
    // sin esto un auto muy rapido podria atravesar edificios
    // body_def.bullet = true;

    b2Body* car = world->CreateBody(&body_def);

    b2PolygonShape boxShape;
    boxShape.SetAsBox(28.0f/2, 22.0/2);

    b2FixtureDef boxFixtureDef;
    boxFixtureDef.shape = &boxShape;
    boxFixtureDef.density = 1;
    // boxFixtureDef.friction = 0.3f;
    car->CreateFixture(&boxFixtureDef);

    car->SetLinearDamping(0.5f);  // para que se frene con el tiempo

    return car;
}

void Game::updatePlayerCars() {
    for (auto& [id, player]: players) {
        player.updateCarPhysics();
    }
}

// TODO:
// El broadcast tiene que mandar que estado de juego es (countdown, racing, etc)
// El cliente tiene que saber interpretar estos estados
void Game::broadcast() {
    auto remaining = getRemainingGameStateTime();

    std::vector<Snapshot::CarSnapshot> snapshots;
    for (auto& [clientId, player]: players) {
        Snapshot::CarSnapshot snp = player.buildCarSnapshot();
        snapshots.emplace_back(snp);
    }

    responseQueuesMonitor.broadcast(
            std::make_shared<Snapshot>(static_cast<uint32_t>(remaining.count()), snapshots));
}

void Game::broadcast_start_signal() {
    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>());  // dummy timestamp for now
}

std::chrono::seconds Game::getRemainingGameStateTime() {
    auto now = std::chrono::high_resolution_clock::now();
    auto gameStateElapsed = now - gameStateStartTime;

    switch (current_state) {
        case game_state::COUNTDOWN:
            return std::chrono::duration_cast<std::chrono::seconds>(
                    countdownDuration - gameStateElapsed);
        case game_state::RACING:
            return std::chrono::duration_cast<std::chrono::seconds>(
                    raceDuration - gameStateElapsed);
        case game_state::SHOWING_STATS:
            return std::chrono::duration_cast<std::chrono::seconds>(
                    statsDuration - gameStateElapsed);
        case game_state::MODIFYING_CAR:
            return std::chrono::duration_cast<std::chrono::seconds>(
                    upgradesDuration - gameStateElapsed);
        case game_state::ELIMINATED:
        case game_state::GAME_END:
            return std::chrono::seconds(0);
    }
    return std::chrono::seconds(0);  // para evitar warning
}

void Game::setGameState(game_state new_state) {
    current_state = new_state;
    gameStateStartTime = std::chrono::high_resolution_clock::now();

    //mandar un snapshot de que cambio el estado?
    //o el cliente simplemente recibe el primer snapshot del nuevo estado?

    /* EJEMPLO: 
    Snapshot snp;  
    snp.phase = newPhase;  
    snp.remaining_ms = getPhaseRemainingMs();

    responseQueuesMonitor.broadcast(
        std::make_shared<Snapshot>(snp)
    ); */
}

void Game::updateGameState() {
    auto now = std::chrono::high_resolution_clock::now();
    auto gameStateElapsed = now - gameStateStartTime;

    switch (current_state) {
        case game_state::COUNTDOWN:
            if (gameStateElapsed >= countdownDuration) {
                setGameState(game_state::RACING);
            }
            break;
        case game_state::RACING:
            if (gameStateElapsed >= raceDuration) {
                setGameState(game_state::SHOWING_STATS);
            }
            break;
        case game_state::SHOWING_STATS:
            if (gameStateElapsed >= statsDuration) {
                setGameState(game_state::MODIFYING_CAR);
            }
            break;
        case game_state::MODIFYING_CAR:
            if (gameStateElapsed >= upgradesDuration) {
                setGameState(game_state::COUNTDOWN);
            }
            break;
        case game_state::ELIMINATED: 
        // eliminated le sirve solo al cliente?
        // si un usuario muere el server le va a estar mandando snapshots de carrera
        // pero tambien manda la vida del auto
        // si el cliente checkea que su vida es 0, pasa a eliminated en vez de racing
        case game_state::GAME_END:
            // ?
            break;
    }
}

void Game::handleGameState(float deltaTime) {
    switch (current_state) {
        case game_state::COUNTDOWN:
            handleCountdownState();
            break;
        case game_state::RACING:
            handleRacingState(deltaTime);
            break;
        case game_state::SHOWING_STATS:
            handleShowingStatsState();
            break;
        case game_state::MODIFYING_CAR:
            handleModifyingCarState();
            break;
        case game_state::ELIMINATED:
            // handleEliminatedState();
            break;
        case game_state::GAME_END:
            handleGameEndState();
            break;
    }
}

void Game::handleCountdownState() {
    // el countdown manda por protocolo 1 snapshot por segundo con el numero del countdown?
    // el gameloop ya manda un snapshot por frame. Creo que es mejor mantenerlo consistente y mandar el segundo actual del contdown por cada frame.
}

void Game::handleRacingState(float deltaTime) {
    // command pattern
    std::unique_ptr<Command> cmd;
    while (clientCommandsQueue.tryPop(cmd)) {
        cmd->execute(*this);
    }

    updatePlayerCars();

    world->Step(deltaTime, velocityIt, positionIt);
}

void Game::handleShowingStatsState() {
    // el handler de showing stats es el que tiene que checkear si
    // ya no quedan más carreras por correr 
    // (para mostrar un ganador y no mostrar la pantalla de mejoras)
}

void Game::handleModifyingCarState() {
    // mandar al cliente las modificaciones disponibles/su magnitud?
    // recibir las modificaciones de los clientes (patron comando de nuevo?)
    /* while (clientCommandsQueue.tryPop(cmd)) {
        cmd->execute(*this);
    } */
    // hay que hacer un nuevo tipo de comando (ModifyCarCommand?) que modifique las propiedades del auto del jugador
}

/**
 * TODO: implement constant rate loop!
 */
void Game::run() {
    broadcast_start_signal();
    setGameState(game_state::COUNTDOWN);

    /* auto collisionBodies =  */CollisionLoader::LoadCollisions("server/gameLogic/collisions.yaml", world, 1.0f, WORLD_HEIGHT);

    using clock = std::chrono::high_resolution_clock;
    auto lastTime = clock::now();

    while (shouldKeepRunning()) {
        updateGameState();

        auto now = clock::now();
        auto elapsed = now - lastTime;
        float deltaTime = elapsed.count();  // segundos
        lastTime = now;
        
        handleGameState(deltaTime);
        
        broadcast();

        // Mantener FPS constante
        auto frameTime =
                std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - lastTime).count();
        if (frameTime < FRAME_DURATION_MS) {
            std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_DURATION_MS - frameTime));
        }
    }
}

void Game::stop() {
    Thread::stop();
    clientCommandsQueue.close();  // Receivers cannot push, game cannot tryPop (once it's empty)
    responseQueuesMonitor.closeAll();  // Senders cannot pop, game cannot tryPush when broadcasting
}

bool Game::isDead() { return !isAlive(); }

Queue<std::unique_ptr<Command>>& Game::getClientCommandsQueue() { return clientCommandsQueue; }

Queue<std::shared_ptr<Snapshot>>& Game::getResponsesQueue(ClientID clientId) {
    return responseQueuesMonitor.getQueue(clientId);
}

void Game::addPlayer(ClientID clientId, const std::string& username, CarID carId) {
    if (players.find(clientId) == players.end()) {
        b2Body* newCarBody = createNewCarBody();
        try {
            players.emplace(clientId, Player(clientId, username, newCarBody, carId));
        } catch (std::exception& e) {
            std::cerr << "Error en addPlayer: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Error en addPlayer: no se" << std::endl;
        }
    }
    responseQueuesMonitor.addQueue(clientId);
}

void Game::movePlayer(ClientID clientId, ActiveDirections activeDirections) {
    players.at(clientId).move(activeDirections);
}

void Game::makeInmortal(ClientID clientId) {
    std::cout << "Making player " << clientId << " inmortal!" << std::endl;
    /**
     * TODO: implementar
     */
}

void Game::makeInstaWin(ClientID clientId) {
    std::cout << "Making player " << clientId << " insta win!" << std::endl;
    /**
     * TODO: implementar
     */
}

void Game::makeInstaLose(ClientID clientId) {
    std::cout << "Making player " << clientId << " insta lose!" << std::endl;
    /**
     * TODO: implementar
     */
}

Game::~Game() {}
