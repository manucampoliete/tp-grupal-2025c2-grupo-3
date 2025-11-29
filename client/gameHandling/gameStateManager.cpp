#include "gameStateManager.h"


void GameStateManager::setCountdown(uint8_t number) {
    currentState = GameState::COUNTDOWN;
    countdownNumber = number;
}

void GameStateManager::startRace() {
    currentState = GameState::RACING;
    eliminatedPopupDelayMs = 0.0f;
}

void GameStateManager::setEliminated() {
    currentState = GameState::ELIMINATED;
    eliminatedPopupDelayMs = 1500.0f;
}

void GameStateManager::showStats(const RaceResults& results) {
    currentState = GameState::SHOWING_STATS;
    raceResults = results;
    statsTimerMs = 10;
}

void GameStateManager::showModifications(const CarProperties& props) {
    currentState = GameState::MODIFYING_CAR;
    ModifyingState::Properties p;
    p.speed = static_cast<float>(props.speed) / 1000;
    p.health = static_cast<float>(props.health) / 100;
    p.accel = static_cast<float>(props.accel) / 1000;
    p.mass = static_cast<float>(props.mass) / 1000;
    carProperties = p;
    speedModified = false;
    healthModified = false;
    accelModified = false;
    massModified = false;
    modsSaved = false;
    modTimerMs = 10;
}

void GameStateManager::showGameEnd(const FinalResults& results) {
    currentState = GameState::GAME_END;
    finalResults = results;
}

void GameStateManager::showCheatNotification(CheatType type) {
    activeCheat = type;
    cheatTimerMs = 1000.0f;
}

void GameStateManager::updateTimers(float dtMs) {
    // Cheat notification timer
    if (cheatTimerMs > 0) {
        cheatTimerMs -= dtMs;
        if (cheatTimerMs <= 0) {
            cheatTimerMs = 0;
            activeCheat = CheatType::NONE;
        }
    }

    // Eliminated popup delay
    if (eliminatedPopupDelayMs > 0) {
        eliminatedPopupDelayMs -= dtMs;
        if (eliminatedPopupDelayMs < 0)
            eliminatedPopupDelayMs = 0;
    }
}

void GameStateManager::toggleSpeedMod() {
    if (!modsSaved)
        speedModified = !speedModified;
}

void GameStateManager::toggleHealthMod() {
    if (!modsSaved)
        healthModified = !healthModified;
}

void GameStateManager::toggleAccelMod() {
    if (!modsSaved)
        accelModified = !accelModified;
}

void GameStateManager::toggleMassMod() {
    if (!modsSaved)
        massModified = !massModified;
}

void GameStateManager::saveMods() {
    modsSaved = true;
}