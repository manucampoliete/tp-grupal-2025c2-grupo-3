#ifndef GAME_H
#define GAME_H

#include <SDL2pp/Font.hh>
#include <SDL2pp/Renderer.hh>
#include <SDL2pp/SDL.hh>
#include <SDL2pp/SDLTTF.hh>
#include <SDL2pp/Texture.hh>
#include <SDL2pp/Window.hh>

#include "../common/messages/gameData.h"
#include "../common/messages/snapshot.h"

#include "eventHandler.h"
#include "interfaceRenderer.h"
#include "soundManager.h"
#include "world.h"
#include "worldRenderer.h"
#include "gameLoop.h"

#include "../common/utils/gameState.h"

using namespace SDL2pp;


class GameLoop;

class Game {
private:
    SDL2pp::SDL sdl;
    SDL2pp::SDLTTF ttf;
    SDL2pp::Window window;
    SDL2pp::Renderer renderer;
    SDL2pp::Font font;
    SDL2pp::Font fontSmall;
    SDL2pp::Font fontBig;

    SDL2pp::Texture mapTexture;
    SDL2pp::Texture carSprites;

    SDL2pp::Texture cheatInmortalityImg;
    SDL2pp::Texture cheatWinImg;
    SDL2pp::Texture cheatLoseImg;

    World& world;
    GameLoop& gameLoop;
    uint8_t playerId;

    bool quit = false;

    EventHandler eventHandler;

    WorldRenderer worldRenderer;
    UIRenderer interfaceRenderer;
    SoundManager soundManager;

    /**
     * TODO: change to GameState::COUNTDOWN when the feature is implemented
     */
    GameState currentState;

    // Countdown
    uint8_t countdownNumber = 100;  // 3, 2, 1, 0=GO
    float countdownTimer = 0.0f;  // Internal timer to change countdown numbers

    uint32_t raceTimerMs;
    uint32_t statsTimerMs;
    uint32_t modTimerMs;

    RaceResults currentResults;
    CarProperties currentProperties;
    FinalResults finalResults;

    int lastWindowWidth = 0;
    int lastWindowHeight = 0;

    // Number of current race and total hardcoded (that info should be sent by the server)
    int currentRace = 1;
    int totalRaces = 6;

    bool speedModified = false;
    bool healthModified = false;
    bool saved = false;

    // Visual effects
    bool screenFlashActive = false;
    float flashTimer = 0.0f;

    CheatType activeCheatNotification = CheatType::NONE;
    float cheatNotificationTimer = 0.0f;

    float eliminatedPopupDelayMs;

    void processInput();
    void update(float dt);
    void render();

public:
    Game(World& world, GameLoop& gameLoop, uint8_t playerId);

    /**
     * Processes a complete frame (input, update, render)
     * Returns false if the game should be closed
     */
    bool processFrame(float dt);

    void startRace();

    bool shouldClose() const { return quit; }

    GameState getCurrentState() const;
    void updateUILayout();

    void checkWindowResize();

    void setRaceTimer(uint32_t timeMs);
    void showCountdown(uint8_t number);
    void showStats(const RaceResults& results);
    void showModifications(const CarProperties& props);
    void showCheatNotification(CheatType cheatType);
    void showFinalResults(const FinalResults& results);

    void onPlayerDied(uint16_t playerId);
    void onCollision(float x, float y, float intensity);
    void triggerScreenFlash();

    SoundManager& getSoundManager() { return soundManager; }
    WorldRenderer& getWorldRenderer() { return worldRenderer; }
    const CarProperties& getCurrentProperties() const { return currentProperties; }
};

#endif  // GAME_H
