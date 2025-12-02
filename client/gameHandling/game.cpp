#include "game.h"

#include <iostream>

#include "../threads/gameLoop.h"

Game::Game(World& world, GameLoop& gameLoop, uint8_t playerId) : 
    sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO),
    ttf(),
    window("Need For Speed", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            800, 600, SDL_WINDOW_RESIZABLE),
    renderer(window, -1, SDL_RENDERER_ACCELERATED),
    
    // Fonts
    #ifdef INSTALL_MODE
        font("/var/needForSpeed2D/client/assets/fonts/VCR_OSD_MONO.ttf", 24),
        fontSmall("/var/needForSpeed2D/client/assets/fonts/VCR_OSD_MONO.ttf", 18),
        fontBig("/var/needForSpeed2D/client/assets/fonts/VCR_OSD_MONO.ttf", 30),

        // Textures
        // mapTexture(renderer, "/var/needForSpeed2D/client/assets/cities/Game Boy _ GBC - Grand Theft Auto - "
        //                    "Backgrounds - Vice City.png"),
        carSprites(renderer,
                    SDL2pp::Surface("/var/needForSpeed2D/client/assets/cars/Mobile - Grand Theft Auto 4 - "
                                    "Miscellaneous - Cars.png")
                        .SetColorKey(true, 0xa3a30d)),
        cheatInmortalityImg(renderer, "/var/needForSpeed2D/client/assets/cheats/inmortality.png"),
        cheatWinImg(renderer, "/var/needForSpeed2D/client/assets/cheats/win.png"),
        cheatLoseImg(renderer, "/var/needForSpeed2D/client/assets/cheats/lose.png"),
        cheatSpeedImg(renderer, "/var/needForSpeed2D/client/assets/cheats/speed.png"),
        finishImg(renderer, "/var/needForSpeed2D/client/assets/cheats/finish.png"),
    #else
        font("client/assets/fonts/VCR_OSD_MONO.ttf", 24),
        fontSmall("client/assets/fonts/VCR_OSD_MONO.ttf", 18),
        fontBig("client/assets/fonts/VCR_OSD_MONO.ttf", 30),

        carSprites(renderer,
                    SDL2pp::Surface("client/assets/cars/Mobile - Grand Theft Auto 4 - "
                                    "Miscellaneous - Cars.png")
                        .SetColorKey(true, 0xa3a30d)),
        cheatInmortalityImg(renderer, "client/assets/cheats/inmortality.png"),
        cheatWinImg(renderer, "client/assets/cheats/win.png"),
        cheatLoseImg(renderer, "client/assets/cheats/lose.png"),
        cheatSpeedImg(renderer, "client/assets/cheats/speed.png"),
        finishImg(renderer, "client/assets/cheats/finish.png"),
    #endif
    
    // References
    world(world),
    gameLoop(gameLoop),
    playerId(playerId),
    
    // Components
    stateManager(),
    soundManager(),
    worldRenderer(renderer, carSprites, world, playerId),
    uiRenderer(renderer, font, fontSmall, fontBig, world, playerId,
                cheatInmortalityImg, cheatWinImg, cheatLoseImg, cheatSpeedImg, finishImg),
    effectsManager(),
    inputHandler(gameLoop, stateManager, soundManager, worldRenderer, uiRenderer) {

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_SetHint(SDL_HINT_AUDIO_RESAMPLING_MODE, "1");
    SDL_SetWindowMinimumSize(window.Get(), 800, 600);

    loadSounds();
    updateUILayout();
}


void Game::loadSounds() {
    try {
        #ifdef INSTALL_MODE
            soundManager.loadSound("collision", "/var/needForSpeed2D/client/assets/sounds/collision.wav");
            soundManager.loadSound("explosion", "/var/needForSpeed2D/client/assets/sounds/explosion.wav");
            soundManager.loadSound("checkpoint", "/var/needForSpeed2D/client/assets/sounds/checkpoint.wav");
            soundManager.loadSound("countdown", "/var/needForSpeed2D/client/assets/sounds/beep.wav");
            soundManager.loadSound("raceEnd", "/var/needForSpeed2D/client/assets/sounds/raceEnd.wav");
            soundManager.loadSound("brake", "/var/needForSpeed2D/client/assets/sounds/brake.wav");
            soundManager.loadSound("engine", "/var/needForSpeed2D/client/assets/sounds/engine.wav");
            soundManager.loadSound("confirm", "/var/needForSpeed2D/client/assets/sounds/confirm.wav");
            soundManager.loadSound("victory", "/var/needForSpeed2D/client/assets/sounds/victory.wav");
            soundManager.loadSound("raceStart", "/var/needForSpeed2D/client/assets/sounds/raceStart.wav");
            soundManager.loadSound("buttonClick", "/var/needForSpeed2D/client/assets/sounds/buttonClick.wav");
        #else
            soundManager.loadSound("collision", "client/assets/sounds/collision.wav");
            soundManager.loadSound("explosion", "client/assets/sounds/explosion.wav");
            soundManager.loadSound("checkpoint", "client/assets/sounds/checkpoint.wav");
            soundManager.loadSound("countdown", "client/assets/sounds/beep.wav");
            soundManager.loadSound("raceEnd", "client/assets/sounds/raceEnd.wav");
            soundManager.loadSound("brake", "client/assets/sounds/brake.wav");
            soundManager.loadSound("engine", "client/assets/sounds/engine.wav");
            soundManager.loadSound("confirm", "client/assets/sounds/confirm.wav");
            soundManager.loadSound("victory", "client/assets/sounds/victory.wav");
            soundManager.loadSound("raceStart", "client/assets/sounds/raceStart.wav");
            soundManager.loadSound("buttonClick", "client/assets/sounds/buttonClick.wav");
        #endif
    } catch (const std::exception& e) {
        std::cerr << "[GAME] Error loading sounds: " << e.what() << std::endl;
    }
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
            if (worldRenderer.hasMapLoaded())
                uiRenderer.renderMinimap(worldRenderer.getMapTexture(), window.GetWidth(), window.GetHeight());
            if (world.hasPlayerFinished(playerId))
                uiRenderer.renderFinishedPopup();
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
        soundManager.playSound("raceStart");
}

void Game::setRaceInfo(const RaceInfo info) {
    mapId = info.mapId;
    race = info.race;
    totalRaces = info.totalRaces;

    std::string mapPath;
    std::string bridgePath;
    std::string musicPath;

    stateManager.setCurrentRace(race);
    stateManager.setTotalRaces(totalRaces);

    #ifdef INSTALL_MODE
        switch (mapId) {
            case MAP_LIBERTY_CITY: // Liberty City
                mapPath = "/var/needForSpeed2D/client/assets/cities/Game Boy _ GBC - Grand Theft Auto - Backgrounds - Liberty City.png";
                bridgePath = "/var/needForSpeed2D/client/assets/cities/Liberty-City-Bridges.png";
                musicPath = "/var/needForSpeed2D/client/assets/sounds/libertyCity.mp3";
                break;
            case MAP_SAN_ANDREAS: // San Andreas
            //    mapPath = "/var/needForSpeed2D/client/assets/cities/Game Boy _ GBC - Grand Theft Auto - Backgrounds - San Andreas.png";
            //    bridgePath = "/var/needForSpeed2D/client/assets/cities/San-Andreas-Bridges.png";
                break;
            case MAP_VICE_CITY: // Vice City
                mapPath = "/var/needForSpeed2D/client/assets/cities/Game Boy _ GBC - Grand Theft Auto - Backgrounds - Vice City.png";
                bridgePath = "/var/needForSpeed2D/client/assets/cities/Vice-City-Bridges.png";
                musicPath = "/var/needForSpeed2D/client/assets/sounds/viceCity.mp3";
                break;
            default:
                std::cerr << "[GAME] Warning: Could not load map " << (int)mapId << std::endl;
                break;
        }
    #else
        switch (mapId) {
            case MAP_LIBERTY_CITY: // Liberty City
                mapPath = "client/assets/cities/Game Boy _ GBC - Grand Theft Auto - Backgrounds - Liberty City.png";
                bridgePath = "client/assets/cities/Liberty-City-Bridges.png";
                musicPath = "client/assets/sounds/libertyCity.mp3";
                break;
            case MAP_SAN_ANDREAS: // San Andreas
            //    mapPath = "client/assets/cities/Game Boy _ GBC - Grand Theft Auto - Backgrounds - San Andreas.png";
            //    bridgePath = "client/assets/cities/San-Andreas-Bridges.png";
                break;
            case MAP_VICE_CITY: // Vice City
                mapPath = "client/assets/cities/Game Boy _ GBC - Grand Theft Auto - Backgrounds - Vice City.png";
                bridgePath = "client/assets/cities/Vice-City-Bridges.png";
                musicPath = "client/assets/sounds/viceCity.mp3";
                break;
            default:
                std::cerr << "[GAME] Warning: Could not load map " << (int)mapId << std::endl;
                break;
        }
    #endif
    

    //load new textures
    // Thanks to unique_ptr the old ones are deleted automatizally
    worldRenderer.loadMapTexture(mapPath);
    worldRenderer.loadBridgeTexture(bridgePath);

    try {
        soundManager.loadMusic(musicPath); 
    } catch (const std::exception& e) {
        std::cerr << "[GAME] Warning: Could not load music for map " << (int)mapId << ": " << e.what() << std::endl;
    }
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
    soundManager.playSound("raceEnd");
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

void Game::onCollision(float x, float y, bool intensity) {
    worldRenderer.addCollisionEffect(x, y, intensity);
    int volume;
    if (intensity)
        volume = static_cast<int>(MIX_MAX_VOLUME);
    else
        volume = static_cast<int>(0.5 * MIX_MAX_VOLUME);

    soundManager.playSound("collision", volume);
    if (intensity) effectsManager.triggerScreenFlash();
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