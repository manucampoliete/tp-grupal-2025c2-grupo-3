#include "game.h"

#include <algorithm>
#include <chrono>
#include <thread>
#include <utility>
#include <vector>

#define TARGET_FPS 60
#define FRAME_DURATION_MS (1000 / TARGET_FPS)
#define PLAYER_SPEED 200.0f  // píxeles por segundo
#define WORLD_HEIGHT 4672

Game::Game():
        world(new b2World(b2Vec2(0, 0))),
        velocityIt(8),
        positionIt(3),
        clientCommandsQueue(),
        responseQueuesMonitor(),
        players() {}

b2Body* Game::createNewCarBody() {
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    // body_def.position.Set(0, 0);
    body_def.position.Set(WORLD_HEIGHT/2, WORLD_HEIGHT/2);
    body_def.angle = 0;
    b2Body* car = world->CreateBody(&body_def);

    b2PolygonShape boxShape;
    boxShape.SetAsBox(3, 1);

    b2FixtureDef boxFixtureDef;
    boxFixtureDef.shape = &boxShape;
    boxFixtureDef.density = 1;
    car->CreateFixture(&boxFixtureDef);

    car->SetLinearDamping(0.5f);  // para que se frene con el tiempo

    return car;
}

void Game::updatePlayerCars() {
    for (auto& [id, player]: players) {
        player.updateCarPhysics();
    }
}

void Game::broadcast() {
    std::vector<Snapshot::CarSnapshot> snapshots;
    for (auto& [clientId, player]: players) {
        Snapshot::CarSnapshot snp = player.buildCarSnapshot();
        snapshots.emplace_back(snp);
    }
    responseQueuesMonitor.broadcast(
            std::make_shared<Snapshot>(0, snapshots));  // dummy timestamp for now
}

void Game::broadcast_start_signal() {
    responseQueuesMonitor.broadcast(
            std::make_shared<Snapshot>());  // dummy timestamp for now
}

/**
 * TODO: implement constant rate loop!
 */
void Game::run() {
    broadcast_start_signal();

    using clock = std::chrono::high_resolution_clock;
    auto lastTime = clock::now();

    while (shouldKeepRunning()) {
        auto now = clock::now();
        std::chrono::duration<float> elapsed = now - lastTime;
        float deltaTime = elapsed.count();  // segundos
        lastTime = now;

        /**
         * Command pattern!
         */
        std::unique_ptr<Command> cmd;
        while (clientCommandsQueue.tryPop(cmd)) {
            cmd->execute(*this);
        }

        updatePlayerCars();

        world->Step(deltaTime, velocityIt, positionIt);

        broadcast();

        // Mantener FPS constante
        auto frameTime =
                std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - now).count();
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

Queue<std::unique_ptr<Command>>& Game::getClientCommandsQueue() { return clientCommandsQueue; }

Queue<std::shared_ptr<Snapshot>>& Game::getResponsesQueue(ClientID clientId) {
    return responseQueuesMonitor.getQueue(clientId);
}

void Game::addPlayer(ClientID clientId, const std::string& username,
                     uint8_t carId) {  // carId unused for now
    (void)carId;

    if (players.find(clientId) == players.end()) {
        b2Body* newCarBody = createNewCarBody();
        players.emplace(clientId, Player(clientId, username, newCarBody));
    }
    responseQueuesMonitor.addQueue(clientId);
}

void Game::movePlayer(ClientID clientId, ActiveDirections activeDirections) {
    players.at(clientId).move(activeDirections);
}

Game::~Game() {}
