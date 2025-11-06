#include "game.h"

#include <algorithm>
#include <chrono>
#include <thread>
#include <utility>
#include <vector>

#define TARGET_FPS 60
#define FRAME_DURATION_MS (1000 / TARGET_FPS)
#define PLAYER_SPEED 200.0f  // píxeles por segundo

Game::Game() : 
        world(new b2World(b2Vec2(0, 0))),
        velocityIt(8),
        positionIt(3),
        clientCommandsQueue(),
        responseQueuesMonitor(),
        players() {}

b2Body* Game::createNewCarBody() {
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position.Set(0, 0);
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

void Game::movePlayer(ClientID clientId, ActiveDirections activeDirections) {
    players.at(clientId).move(activeDirections);
}

void Game::updatePlayerCars() {
    for (auto& [id, player]: players) {
        player.updateCarPhysics();
    }
}

void Game::broadcast() {
    std::vector<Snapshot::CarSnapshot> snapshots;
    for (auto& [client_id, player]: players) {
        Snapshot::CarSnapshot snp = player.buildCarSnapshot();
        snapshots.emplace_back(snp);
    }
    Snapshot snapshot(0, snapshots);
    responseQueuesMonitor.broadcast(std::make_shared<Snapshot>(snapshot));
}

/**
 * TODO: implement constant rate loop
 */
void Game::run() {
    using clock = std::chrono::high_resolution_clock;
    auto last_time = clock::now();

    while (shouldKeepRunning()) {
        auto now = clock::now();
        std::chrono::duration<float> elapsed = now - last_time;
        float delta_time = elapsed.count();  // segundos
        last_time = now;

        /**
         * Command pattern
         */
        std::unique_ptr<Command> cmd;
        while (clientCommandsQueue.tryPop(cmd)) {
            cmd->execute(*this);
        }

        updatePlayerCars();

        world->Step(delta_time, velocityIt, positionIt);

        broadcast();

        // Mantener FPS constante
        auto frame_time =
                std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - now).count();
        if (frame_time < FRAME_DURATION_MS) {
            std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_DURATION_MS - frame_time));
        }
    }
}

void Game::stop() {
    Thread::stop();
    clientCommandsQueue.close();    // Receivers cannot push, game cannot try_pop (once it's empty)
    responseQueuesMonitor.closeAll();  // Senders cannot pop, game cannot try_push when broadcasting
}

Queue<std::unique_ptr<Command>>& Game::getClientCommandsQueue() { return clientCommandsQueue; }

Queue<std::shared_ptr<Snapshot>>& Game::getResponsesQueue(ClientID clientId) {
    return responseQueuesMonitor.getQueue(clientId);
}

void Game::addPlayer(ClientID client_id, const std::string& username, uint8_t car_id) {
    if (players.find(client_id) == players.end()) {
        b2Body* new_car_body = createNewCarBody();
        players.emplace(client_id, Player(client_id, username, new_car_body));
    }
    responseQueuesMonitor.addQueue(client_id);
}

Game::~Game() {}
