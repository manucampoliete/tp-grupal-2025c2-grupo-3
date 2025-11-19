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
#define TIME_STEP (1.0f / TARGET_FPS) // duracion del step que simula box2d cada frame

Game::Game():
        world(new b2World(b2Vec2(0, 0))),
        velocityIt(8),
        positionIt(3),
        clientCommandsQueue(),
        responseQueuesMonitor(),
        players(),
        countdownDuration(15),
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

void Game::broadcastCountdown() {
    auto remaining = getRemainingGameStateTime();
    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(static_cast<uint32_t>(remaining.count())));
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

void Game::broadcast() {
    switch (current_state) {
        case GameState::COUNTDOWN:
            broadcastCountdown();
            break;
        case GameState::RACING: {
            broadcastRacing();
            break;
        }
        case GameState::SHOWING_STATS:
        case GameState::MODIFYING_CAR:
        case GameState::ELIMINATED:
        case GameState::GAME_END:
            // implementar cuando haga falta
            break;
    }
}

void Game::broadcast_start_signal() {
    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>());  // dummy timestamp for now
}

std::chrono::seconds Game::getRemainingGameStateTime() {
    auto now = std::chrono::high_resolution_clock::now();
    auto gameStateElapsed = now - gameStateStartTime;

    switch (current_state) {
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

void Game::setGameState(GameState new_state) {
    current_state = new_state;
    gameStateStartTime = std::chrono::high_resolution_clock::now();

    // el cliente espera que le avisen cuando cambia el estado
    switch (new_state) {
        case GameState::COUNTDOWN:
            std::cout << "[GAME] Estado cambiado a COUNTDOWN" << std::endl;
            break;
        case GameState::RACING:
            std::cout << "[GAME] Estado cambiado a RACING" << std::endl;
            responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(MSG_RACE_START));
            break;
        case GameState::SHOWING_STATS:
            std::cout << "[GAME] Estado cambiado a SHOWING_STATS" << std::endl;
            break;
        case GameState::MODIFYING_CAR:
            std::cout << "[GAME] Estado cambiado a MODIFYING_CAR" << std::endl;
            break;
        case GameState::ELIMINATED:
            std::cout << "[GAME] Estado cambiado a ELIMINATED" << std::endl;
            break;
        case GameState::GAME_END:
            std::cout << "[GAME] Estado cambiado a GAME_END" << std::endl;
            break;
    }
}

void Game::updateGameState() {
    auto now = std::chrono::high_resolution_clock::now();
    auto gameStateElapsed = now - gameStateStartTime;

    switch (current_state) {
        case GameState::COUNTDOWN:
            if (gameStateElapsed >= countdownDuration) {
                setGameState(GameState::RACING);
            }
            break;
        case GameState::RACING:
            if (gameStateElapsed >= raceDuration) {
                setGameState(GameState::SHOWING_STATS);
            }
            break;
        case GameState::SHOWING_STATS:
            if (gameStateElapsed >= statsDuration) {
                setGameState(GameState::MODIFYING_CAR);
            }
            break;
        case GameState::MODIFYING_CAR:
            if (gameStateElapsed >= upgradesDuration) {
                setGameState(GameState::COUNTDOWN);
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
    switch (current_state) {
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
    // command pattern
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
    // mandar al cliente las modificaciones disponibles/su magnitud?
    // recibir las modificaciones de los clientes (patron comando de nuevo?)
    /* while (clientCommandsQueue.tryPop(cmd)) {
        cmd->execute(*this);
    } */
    // hay que hacer un nuevo tipo de comando (ModifyCarCommand?) que modifique las propiedades del auto del jugador
}

void Game::run() {
    broadcast_start_signal();
    setGameState(GameState::COUNTDOWN);

    /* auto collisionBodies =  */CollisionLoader::LoadCollisions("server/gameLogic/collisions.yaml", world, 1.0f, WORLD_HEIGHT);

    using clock = std::chrono::high_resolution_clock;
    auto lastTime = clock::now();
    float accumulatedTime = 0.0f;

    while (shouldKeepRunning()) {
        updateGameState();

        auto now = clock::now();
        float frameTime = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        accumulatedTime += frameTime;
        
        while (accumulatedTime >= TIME_STEP) {
            handleGameState();
            accumulatedTime -= TIME_STEP;
        }

        broadcast();

        // evita busy waiting
        float remaining = TIME_STEP - accumulatedTime;
        if (remaining > 0.0f) {
            // convierto a microsegundos para mas precision, busca evitar el jitter
            auto micros = std::chrono::microseconds(static_cast<int64_t>(remaining * 1'000'000));
            std::this_thread::sleep_for(micros);
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
