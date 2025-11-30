#ifndef GAME_H
#define GAME_H

#include <SDL2pp/Font.hh>
#include <SDL2pp/Renderer.hh>
#include <SDL2pp/SDL.hh>
#include <SDL2pp/SDLTTF.hh>
#include <SDL2pp/Texture.hh>
#include <SDL2pp/Window.hh>

#include "world.h"
#include "gameStateManager.h"
#include "inputHandler.h"
#include "../audio/soundManager.h"
#include "../rendering/worldRenderer.h"
#include "../rendering/interfaceRenderer.h"
#include "../rendering/effectsManager.h"

#include "../../common/utils/mapConstants.h"

using namespace SDL2pp;

class GameLoop;

/**
 * Game: 
 * - Initialize and own SDL resources
 * - Coordniate InputHandler, Renderers, SoundManager, EffectsManager
 * 
 * Delegates to:
 * - GameStateManager: game phase and timers
 * - InputHandler: SDL event processing
 * - WorldRenderer: world rendering (map, cars, particles)
 * - UIRenderer: UI rendering (HUD, popups, minimap)
 * - EffectsManager: screen-level effects
 * - SoundManager: audio
 */
class Game {
private:
    SDL2pp::SDL sdl;
    SDL2pp::SDLTTF ttf;
    SDL2pp::Window window;
    SDL2pp::Renderer renderer;
    SDL2pp::Font font;
    SDL2pp::Font fontSmall;
    SDL2pp::Font fontBig;

    SDL2pp::Texture carSprites;

    SDL2pp::Texture cheatInmortalityImg;
    SDL2pp::Texture cheatWinImg;
    SDL2pp::Texture cheatLoseImg;
    SDL2pp::Texture cheatSpeedImg;
    SDL2pp::Texture finishImg;

    World& world;
    GameLoop& gameLoop;
    uint8_t playerId;

    uint8_t mapId;
    uint8_t race;
    uint8_t totalRaces;

    // owner
    GameStateManager stateManager;
    SoundManager soundManager;
    WorldRenderer worldRenderer;
    UIRenderer uiRenderer;
    EffectsManager effectsManager;
    InputHandler inputHandler;

    int lastWindowWidth = 0;
    int lastWindowHeight = 0;

    void loadSounds();
    void updateUILayout();
    void update(float dt);
    void render();

public:
    Game(World& world, GameLoop& gameLoop, uint8_t playerId);

    /**
     * Processes a complete frame (input, update, render)
     * Returns false if the game should be closed
     */
    bool processFrame(float dt);

    void showCountdown(uint8_t number);
    void setRaceInfo(const RaceInfo info);
    void startRace();
    void setStatsCountdown(uint8_t number);
    void showStats(const RaceResults& results);
    void showModifications(const std::vector<CarProperties>& props);
    void setModCountdown(uint8_t number);
    void showFinalResults(const FinalResults& results);
    void showCheatNotification(CheatType type);
    
    void onCollision(float x, float y, bool intensity);
    void onPlayerDied(uint16_t deadPlayerId);

    SoundManager& getSoundManager() { return soundManager; }
    GameStateManager& getStateManager() { return stateManager; }
};

#endif  // GAME_H