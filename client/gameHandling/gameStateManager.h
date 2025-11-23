#ifndef GAME_STATE_MANAGER_H
#define GAME_STATE_MANAGER_H

#include <cstdint>

#include "../../common/utils/gameState.h"
#include "../utils/gameData.h"


/**
 * GameStateManager: manages game phase transitions and data
 * - Current game phase
 * - Timer for each phase
 * - Mod phase state 
 * - Race/final results storage
 * - Cheat notification state
 */
class GameStateManager {
private:
    GameState currentState = GameState::COUNTDOWN;

    // Countdown phase
    uint8_t countdownNumber = 100;

    // Racing phase
    uint32_t raceTimerMs = 0;
    int currentRace = 1;
    int totalRaces = 6;

    // Stats phase
    uint8_t statsTimerMs = 10;
    RaceResults raceResults;

    // Modification phase
    uint8_t modTimerMs = 10;
    CarProperties carProperties;
    bool speedModified = false;
    bool healthModified = false;
    bool modsSaved = false;

    // Eliminated state
    float eliminatedPopupDelayMs = 0.0f;

    // Final results
    FinalResults finalResults;

    // Cheat notifications
    CheatType activeCheat = CheatType::NONE;
    float cheatTimerMs = 0.0f;

public:
    GameStateManager() = default;

    // State transitions
    void setCountdown(uint8_t number);
    void startRace();
    void setEliminated();
    void showStats(const RaceResults& results);
    void showModifications(const CarProperties& props);
    void showGameEnd(const FinalResults& results);
    void showCheatNotification(CheatType type);

    // Timer updates
    void updateTimers(float dtMs);

    // Modification controls
    void toggleSpeedMod();
    void toggleHealthMod();
    void saveMods();

    // Timer setters 
    void setRaceTimer(uint32_t ms) { raceTimerMs = ms; }
    void setStatsTimer(uint8_t ms) { statsTimerMs = ms; }
    void setModTimer(uint8_t ms) { modTimerMs = ms; }

    GameState getState() const { return currentState; }
    uint8_t getCountdownNumber() const { return countdownNumber; }
    uint32_t getRaceTimerMs() const { return raceTimerMs; }
    uint8_t getStatsTimerMs() const { return statsTimerMs; }
    uint8_t getModTimerMs() const { return modTimerMs; }
    int getCurrentRace() const { return currentRace; }
    int getTotalRaces() const { return totalRaces; }
    
    bool isSpeedModified() const { return speedModified; }
    bool isHealthModified() const { return healthModified; }
    bool areModsSaved() const { return modsSaved; }
    
    float getEliminatedPopupDelayMs() const { return eliminatedPopupDelayMs; }
    
    CheatType getActiveCheat() const { return activeCheat; }
    
    const RaceResults& getRaceResults() const { return raceResults; }
    const CarProperties& getCarProperties() const { return carProperties; }
    const FinalResults& getFinalResults() const { return finalResults; }
};

#endif  // GAME_STATE_MANAGER_H