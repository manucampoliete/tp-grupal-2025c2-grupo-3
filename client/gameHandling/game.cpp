#include "game.h"

#include <iostream>

#include "../threads/gameLoop.h"

Game::Game(World& world, GameLoop& gameLoop, uint8_t playerId)
    : sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO),
      ttf(),
      window("Need For Speed", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
             800, 600, SDL_WINDOW_RESIZABLE),
      renderer(window, -1, SDL_RENDERER_ACCELERATED),
      
      // Fonts
      font("client/assets/fonts/VCR_OSD_MONO.ttf", 24),
      fontSmall("client/assets/fonts/VCR_OSD_MONO.ttf", 18),
      fontBig("client/assets/fonts/VCR_OSD_MONO.ttf", 30),
      
      // Textures
      mapTexture(renderer, "client/assets/cities/Game Boy _ GBC - Grand Theft Auto - "
                           "Backgrounds - Vice City.png"),
      carSprites(renderer,
                 SDL2pp::Surface("client/assets/cars/Mobile - Grand Theft Auto 4 - "
                                 "Miscellaneous - Cars.png")
                     .SetColorKey(true, 0xa3a30d)),
      cheatInmortalityImg(renderer, "client/assets/cheats/inmortality.png"),
      cheatWinImg(renderer, "client/assets/cheats/win.png"),
      cheatLoseImg(renderer, "client/assets/cheats/lose.png"),
      cheatSpeedImg(renderer, "client/assets/cheats/speed.png"),
      
      // References
      world(world),
      gameLoop(gameLoop),
      playerId(playerId),
      
      // Components
      stateManager(),
      soundManager(),
      worldRenderer(renderer, mapTexture, carSprites, world, playerId),
      uiRenderer(renderer, font, fontSmall, fontBig, mapTexture, world, playerId,
                 cheatInmortalityImg, cheatWinImg, cheatLoseImg, cheatSpeedImg),
      effectsManager(),
      inputHandler(gameLoop, stateManager, soundManager, worldRenderer, uiRenderer) {

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_SetHint(SDL_HINT_AUDIO_RESAMPLING_MODE, "1");
    SDL_SetWindowMinimumSize(window.Get(), 800, 600);

    loadSounds();
    loadBridges();
    updateUILayout();
}


void Game::loadSounds() {
    try {
        soundManager.loadMusic("client/assets/sounds/music.mp3");
        soundManager.loadSound("collision", "client/assets/sounds/collision.wav");
        soundManager.loadSound("explosion", "client/assets/sounds/explosion.wav");
        soundManager.loadSound("checkpoint", "client/assets/sounds/checkpoint.wav");
        soundManager.loadSound("countdown", "client/assets/sounds/beep.wav");
        soundManager.loadSound("race_end", "client/assets/sounds/race_end.wav");
        soundManager.loadSound("brake", "client/assets/sounds/brake.wav");
        soundManager.loadSound("engine", "client/assets/sounds/engine.wav");
        soundManager.loadSound("confirm", "client/assets/sounds/confirm.wav");
        soundManager.loadSound("victory", "client/assets/sounds/victory.wav");
        soundManager.loadSound("race_start", "client/assets/sounds/race_start.wav");
        soundManager.loadSound("button_click", "client/assets/sounds/button_click.wav");
    } catch (const std::exception& e) {
        std::cerr << "[GAME] Error loading sounds: " << e.what() << std::endl;
    }
}

void Game::loadBridges() {
    // por ahora hardcodeado para Vice City
    // TODO: recibir qué mapa se está usando
    std::string bridgePath = "client/assets/cities/Vice-City-Bridges.png";
    worldRenderer.loadBridgeTexture(bridgePath);
}

void Game::updateUILayout() {
    int w = window.GetWidth();
    int h = window.GetHeight();
    worldRenderer.updateLayout(w, h);
    uiRenderer.updateLayout(w, h);
    lastWindowWidth = w;
    lastWindowHeight = h;
}


bool Game::processFrame(float dt) {
    bool windowResized = false;
    if (!inputHandler.processEvents(windowResized))
        return false; 

    if (windowResized) updateUILayout();

    update(dt);
    render();

    return true;
}


void Game::update(float dt) {
    int w = window.GetWidth();
    int h = window.GetHeight();
    if (w != lastWindowWidth || h != lastWindowHeight)
        updateUILayout();

    // Update components
    stateManager.updateTimers(dt);
    worldRenderer.updateEffects(dt / 1000.0f);
    effectsManager.update(dt / 1000.0f);

    // Update race timer from world
    stateManager.setRaceTimer(world.getCountdown());

    // Update camera based on player position
    const auto& cars = world.getCars();
    auto it = cars.find(playerId);
    if (it != cars.end())
        worldRenderer.updateCamera(it->second.x, it->second.y);
}


void Game::render() {
    renderer.Clear();

    // World (map, cars, particles)
    worldRenderer.render();

    // Screen effects 
    effectsManager.render(renderer, window.GetWidth(), window.GetHeight());

    // UI based on current state
    GameState state = stateManager.getState();

    switch (state) {
        case GameState::COUNTDOWN:
            uiRenderer.renderCountdown(stateManager.getCountdownNumber());
            break;

        case GameState::RACING: {
            uiRenderer.renderRaceUI(stateManager.getRaceTimerMs(), stateManager.getCurrentRace(), stateManager.getTotalRaces(), window.GetWidth());
            uint8_t playerHealth = world.getPlayerHealth(playerId);
            uiRenderer.renderHealthBar(playerHealth, window.GetWidth());
            uiRenderer.renderMinimap();
            break;
        }
        case GameState::ELIMINATED:
            if (stateManager.getEliminatedPopupDelayMs() <= 0)
                uiRenderer.renderEliminatedPopup();
            break;

        case GameState::SHOWING_STATS:
            uiRenderer.renderStatsPopup(stateManager.getRaceResults(), stateManager.getStatsTimerMs());
            break;

        case GameState::MODIFYING_CAR:
            uiRenderer.renderModificationPopup(stateManager.isSpeedModified(), stateManager.isHealthModified(), 
                        stateManager.isAccelModified(), stateManager.isMassModified(), 
                        stateManager.areModsSaved(), stateManager.getModTimerMs(), stateManager.getCarProperties());
            break;

        case GameState::GAME_END:
            uiRenderer.renderPodium(stateManager.getFinalResults());
            break;
    }

    // Cheat notification overlay
    if (stateManager.getActiveCheat() != CheatType::NONE) 
        uiRenderer.renderCheatNotification(stateManager.getActiveCheat());

    renderer.Present();
}


void Game::showCountdown(uint8_t number) {
    stateManager.setCountdown(number);
    
    if (number >= 1 && number <= 3)
        soundManager.playSound("countdown");
    else if (number == 0)
        soundManager.playSound("race_start");
}

void Game::startRace() {
    stateManager.startRace();
    soundManager.playMusic();
}

void Game::setStatsCountdown(uint8_t number) {
    stateManager.setStatsTimer(number);
}

void Game::showStats(const RaceResults& results) {
    stateManager.showStats(results);
    soundManager.stopMusic();
    soundManager.playSound("race_end");
}

void Game::showModifications(const std::vector<CarProperties>& props) {
    for (const auto& prop : props) {
        if (prop.playerId == playerId) {
            stateManager.showModifications(prop);
            break;
        }
    }
}

void Game::setModCountdown(uint8_t number) {
    stateManager.setModTimer(number);
}

void Game::showFinalResults(const FinalResults& results) {
    stateManager.showGameEnd(results);
    soundManager.stopMusic();
    
    if (results.winnerId == playerId)
        soundManager.playSound("victory");
}

void Game::showCheatNotification(CheatType type) {
    stateManager.showCheatNotification(type);
}

void Game::onCollision(float x, float y, float intensity) {
    worldRenderer.addCollisionEffect(x, y, intensity);
    
    int volume = static_cast<int>(intensity * MIX_MAX_VOLUME);
    soundManager.playSound("collision", volume);

    if (intensity > 0.7f)
        effectsManager.triggerScreenFlash();
}

void Game::onPlayerDied(uint16_t deadPlayerId) {
    const auto& cars = world.getCars();
    auto it = cars.find(deadPlayerId);
    if (it != cars.end()) {
        worldRenderer.addExplosion(it->second.x, it->second.y, 50);
        soundManager.playSound("explosion");
    }

    if (deadPlayerId == playerId) {
        stateManager.setEliminated();
        soundManager.pauseMusic();
    }
}