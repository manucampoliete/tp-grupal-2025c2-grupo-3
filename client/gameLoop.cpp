#include "gameLoop.h"

#include <iostream>
#include <utility>

#include "game.h"

#define WORLD_HEIGHT 4672.0f


GameLoop::GameLoop(Queue<Snapshot>& serverSnapshotsQueue, Queue<ActiveDirections>& clientRequestsQueue,
                   ClientProtocol& protocol, World& world, ClientID clientId):
        serverSnapshotsQueue(serverSnapshotsQueue),
        clientRequestsQueue(clientRequestsQueue),
        protocol(protocol),
        world(world),
        clientId(clientId),
        game(nullptr) {}

void GameLoop::run() {
    game = std::make_unique<Game>(world, *this, clientId);

    std::cout << "[GAME_LOOP] Initiating SDL game loop..." << std::endl;

    const int FRAME_RATE = 60;
    const float FRAME_TIME_MS = 1000.0f / FRAME_RATE;

    float t1 = SDL_GetTicks();
    bool isRunning = true;

    while (isRunning && shouldKeepRunning()) {
        // Consume all snapshots but apply only the latest one
        Snapshot latestSnapshot;
        bool hasSnapshot = false;

        {
            Snapshot temp;
            while (serverSnapshotsQueue.tryPop(temp)) {
                latestSnapshot = std::move(temp);
                hasSnapshot = true;
            }
        }
        
        // Only apply the latest received snapshot
        if (hasSnapshot) {
            BroadcastData data;

            for (const auto& carSnap: latestSnapshot.cars) {
                BroadcastData::CarState carState;
                carState.id = carSnap.id;
                carState.x = carSnap.x / 1000.0f;
                carState.y = WORLD_HEIGHT - carSnap.y / 1000.0f;
                carState.angle = carSnap.angle + 90.0f;
                carState.type = carSnap.carId;

                data.cars.push_back(carState);
            }

            data.countdown = latestSnapshot.countdown;
            world.update(data);
        }

        // Process frame (input + update + render)
        isRunning = game->processFrame(FRAME_TIME_MS);

        // Synchronize with frame rate
        float t2 = SDL_GetTicks();
        float rest = FRAME_TIME_MS - (t2 - t1);

        if (rest < 0) {
            float behind = -rest;
            rest = FRAME_TIME_MS - fmod(behind, FRAME_TIME_MS);
            float lost = behind + rest;
            t1 += lost;
        }

        SDL_Delay(static_cast<Uint32>(rest));
        t1 += FRAME_TIME_MS;
    }

    std::cout << "[GAME_LOOP] Thread ended." << std::endl;
}


void GameLoop::onCountdown(uint8_t number) {
    if (game)
        game->showCountdown(number);
}

void GameLoop::onRaceStart() {
    if (game)
        game->startRace();
}


// CORREGIR!
void GameLoop::onCheckpointCrossed(uint8_t checkpointId) {
    std::cout << "[GAME_HANDLER] Checkpoint " << (int)checkpointId << " crossed!" << std::endl;

    // A chequear
    
    /**
     * TODO: add visual effect
     */

    if (game)
        game->getSoundManager().playSound("checkpoint");
}


void GameLoop::onCollision(const CollisionData& collision) {
    std::cout << "[GAME_LOOP] Collision detected, intensity: " << collision.intensity << std::endl;
    if (!game)
        return;

    // Coordinates from server in mm to meters
    float worldX = collision.x / 1000.0f;
    float worldY = WORLD_HEIGHT - (collision.y / 1000.0f);
    game->onCollision(worldX, worldY, collision.intensity);
}

void GameLoop::onPlayerDied(ClientID deadPlayerId) {
    if (game)
        game->onPlayerDied(deadPlayerId);
}

void GameLoop::onRaceEnd(const RaceResults& results) {
    if (game)
        game->showStats(results);
}

void GameLoop::onModificationPhase(const CarProperties& props) {
    if (game)
        game->showModifications(props);
}

void GameLoop::onGameEnd(const FinalResults& results) {
    if (game)
        game->showFinalResults(results);
}


void GameLoop::sendMovement(bool up, bool down, bool left, bool right) {
    clientRequestsQueue.tryPush(ActiveDirections(up, down, left, right));
}

void GameLoop::sendModifications(bool speed, bool health) {
    std::cout << "[GAME_HANDLER] Modifications: speed=" << speed << ", health=" << health
              << std::endl;
    protocol.sendModifications(speed, health);
}

void GameLoop::sendCheatInmortality() {
    protocol.sendInmortalityRequest();
    if (game)
        game->showCheatNotification(CheatType::INMORTALITY);
}

void GameLoop::sendCheatInstaWin() {
    protocol.sendInstaWinRequest();
    if (game)
        game->showCheatNotification(CheatType::INSTA_WIN);
}

void GameLoop::sendCheatInstaLose() {
    protocol.sendInstaLoseRequest();
    if (game)
        game->showCheatNotification(CheatType::INSTA_LOSE);
}
