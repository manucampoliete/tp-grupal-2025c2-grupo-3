#include "gameLoop.h"

#include <iostream>
#include <utility>

#include "../../common/constantRateLoop/constantRateLoop.h"
#include "../gameHandling/game.h"

#define WORLD_HEIGHT 4672.0f


GameLoop::GameLoop(Queue<ServerMessage>& serverMessagesQueue, 
                   Queue<ClientMessage>& clientCommandQueue,
                   ClientID clientId):
        serverMessagesQueue(serverMessagesQueue),
        clientCommandQueue(clientCommandQueue),
        clientId(clientId),
        game(nullptr) {}


void GameLoop::run() {
    game = std::make_unique<Game>(world, *this, clientId);

    ConstantRateLoop constantRateLoop;
    bool isRunning = true;

    while (isRunning && shouldKeepRunning()) {
        processServerMessages();
        
        // Process frame (input + update + render)
        isRunning = game->processFrame(RATE);  // RATE from constantRateLoop.h

        // Synchronize with frame rate
        constantRateLoop.sleepAndCalcIt();
    }
}

void GameLoop::processServerMessages() {
    ServerMessage msg;
    Snapshot latestSnapshot;
    bool hasSnapshot = false;

    // Process all the messages from the queue
    while (serverMessagesQueue.tryPop(msg)) {
        // Visitor pattern to process every message type
        std::visit([this, &latestSnapshot, &hasSnapshot](auto&& message) {
            using T = std::decay_t<decltype(message)>;
            
            if constexpr (std::is_same_v<T, Snapshot>) {
                // Only for snapshots, we save the last one
                latestSnapshot = message;
                hasSnapshot = true;
                
            } else if constexpr (std::is_same_v<T, CountdownMessage>) {
                onCountdown(message.number);

            } else if constexpr (std::is_same_v<T, RaceStartMessage>) {
                onRaceStart();
                
            } else if constexpr (std::is_same_v<T, CheckpointMessage>) {
                onCheckpointCrossed(message.checkpointId);
                
            } else if constexpr (std::is_same_v<T, CollisionMessage>) {
                onCollision(message.data);
                
            } else if constexpr (std::is_same_v<T, PlayerDiedMessage>) {
                onPlayerDied(message.playerId);
                
            } else if constexpr (std::is_same_v<T, RaceEndMessage>) {
                onRaceEnd(message.results);

            } else if constexpr (std::is_same_v<T, StatsCountdownMessage>) {
                onStatsCountdown(message.number);

            } else if constexpr (std::is_same_v<T, ModificationPhaseMessage>) {
                onModificationPhase(message.properties);
            
            } else if constexpr (std::is_same_v<T, ModCountdownMessage>) {
                onModCountdown(message.number);
                
            } else if constexpr (std::is_same_v<T, GameEndMessage>) {
                onGameEnd(message.results);
            }
        }, msg);
    }
    
    // We aplly only the last snapshot received
    if (hasSnapshot) 
        applySnapshot(latestSnapshot);
}

void GameLoop::applySnapshot(const Snapshot& snapshot) {
    BroadcastData data;

    for (const auto& carSnap: snapshot.cars) {
        BroadcastData::CarState carState;
        carState.id = carSnap.id;
        carState.x = carSnap.x / 1000.0f;
        carState.y = WORLD_HEIGHT - carSnap.y / 1000.0f;
        carState.angle = carSnap.angle + 90.0f;
        carState.type = carSnap.carId;

        data.cars.push_back(carState);
    }

    data.countdown = snapshot.countdown;
    world.update(data);
}


void GameLoop::onCountdown(uint8_t number) {
    if (number != lastCountdownNumber) {
        lastCountdownNumber = number;
        if (game) game->showCountdown(number);
    }
}

void GameLoop::onRaceStart() {
    if (game) game->startRace();
}


// CORREGIR!
void GameLoop::onCheckpointCrossed(uint8_t checkpointId) {
    std::cout << "[GAME_LOOP] Checkpoint " << (int)checkpointId << " crossed!" << std::endl;

    // A chequear
    
    /**
     * TODO: add visual effect
     */

    // if (game) game->getSoundManager().playSound("checkpoint");
}


void GameLoop::onCollision(const CollisionData& collision) {
    std::cout << "[GAME_LOOP] Collision detected, intensity: " << collision.intensity << std::endl;
    if (!game) return;

    // Coordinates from server in mm to meters
    float worldX = collision.x / 1000.0f;
    float worldY = WORLD_HEIGHT - (collision.y / 1000.0f);
    game->onCollision(worldX, worldY, collision.intensity);
}

void GameLoop::onPlayerDied(ClientID deadPlayerId) {
    if (game) game->onPlayerDied(deadPlayerId);
}

void GameLoop::onRaceEnd(const RaceResults& results) {
    if (game) game->showStats(results);
}

void GameLoop::onStatsCountdown(uint8_t number) {
    if (number != lastStatsCountdown) {
        lastStatsCountdown = number;
        if (game) game->setStatsCountdown(number);
    }
}

void GameLoop::onModificationPhase(const CarProperties& props) {
    if (game) game->showModifications(props);
}

void GameLoop::onModCountdown(uint8_t number) {
    if (number != lastModCountdown) {
        lastModCountdown = number;
        if (game) game->setModCountdown(number);
    }
}

void GameLoop::onGameEnd(const FinalResults& results) {
    if (game) game->showFinalResults(results);
}


void GameLoop::sendMovement(bool up, bool down, bool left, bool right) {
    MoveCommand cmd(ActiveDirections(up, down, left, right));
    clientCommandQueue.tryPush(cmd);
}

void GameLoop::sendModifications(bool speed, bool health) {
    std::cout << "[GAME_HANDLER] Modifications: speed=" << speed << ", health=" << health
              << std::endl;
    ModifyCarCommand cmd(speed, health);
    clientCommandQueue.tryPush(cmd);
}

void GameLoop::sendCheatInmortality() {
    clientCommandQueue.tryPush(CheatInmortalityCommand{});
    if (game) game->showCheatNotification(CheatType::INMORTALITY);
}

void GameLoop::sendCheatInstaWin() {
    clientCommandQueue.tryPush(CheatInstaWinCommand{});
    if (game) game->showCheatNotification(CheatType::INSTA_WIN);
}

void GameLoop::sendCheatInstaLose() {
    clientCommandQueue.tryPush(CheatInstaLoseCommand{});
    if (game) game->showCheatNotification(CheatType::INSTA_LOSE);
}