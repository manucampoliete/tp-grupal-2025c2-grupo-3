#include "game.h"

#include <SDL2pp/SDL.hh>
#include <SDL2pp/SDL2pp.hh>

Game::Game(World& world, GameLoop& gameLoop, uint8_t playerId):
        sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO),
        ttf(),
        window("Need For Speed", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,
               SDL_WINDOW_RESIZABLE),
        renderer(window, -1, SDL_RENDERER_ACCELERATED),
        

        /**
         * FONTS
         */
        font("client/assets/fonts/VCR_OSD_MONO.ttf", 24),
        fontSmall("client/assets/fonts/VCR_OSD_MONO.ttf", 18),
        fontBig("client/assets/fonts/VCR_OSD_MONO.ttf", 30),
        

        /**
         * TEXTURES
         */
        /**
         * Upload map texture from a file
         * For now, harcoded
         * TODO: change when the editor is ready
         */
        mapTexture(renderer, "client/assets/cities/Game Boy _ GBC - Grand Theft Auto - "
                              "Backgrounds - Vice City.png"),

        // Upload car sprites texture
        carSprites(
                renderer,
                SDL2pp::Surface(
                        "client/assets/cars/Mobile - Grand Theft Auto 4 - Miscellaneous - Cars.png")
                        .SetColorKey(true, 0xa3a30d)),
        
        cheatInmortalityImg(renderer, "client/assets/cheats/inmortality.png"),
        cheatWinImg(renderer, "client/assets/cheats/win.png"),
        cheatLoseImg(renderer, "client/assets/cheats/lose.png"),
        
        
        world(world),
        gameLoop(gameLoop),
        playerId(playerId),
        eventHandler(gameLoop, *this),
        worldRenderer(renderer, mapTexture, carSprites, world, playerId),
        interfaceRenderer(renderer, font, fontSmall, fontBig, mapTexture, world, playerId,
                           cheatInmortalityImg, cheatWinImg, cheatLoseImg),
        soundManager() {

    eliminatedPopupDelayMs = 0.0f;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_SetHint(SDL_HINT_AUDIO_RESAMPLING_MODE, "1");
    SDL_SetWindowMinimumSize(window.Get(), 800, 600);

    try {
        soundManager.loadMusic("client/assets/sounds/music.mp3");
        soundManager.loadSound("collision", "client/assets/sounds/crash.wav");
        soundManager.loadSound("explosion", "client/assets/sounds/explosion.wav");
        soundManager.loadSound("checkpoint", "client/assets/sounds/checkpoint.wav");
        soundManager.loadSound("countdown", "client/assets/sounds/beep.wav");
        soundManager.loadSound("race_end", "client/assets/sounds/finish.wav");
        soundManager.loadSound("brake", "client/assets/sounds/brake.wav");
        soundManager.loadSound("engine", "client/assets/sounds/engine.wav");
        soundManager.loadSound("confirm", "client/assets/sounds/confirm.wav");
        soundManager.loadSound("victory", "client/assets/sounds/victory.wav");
        soundManager.loadSound("race_start", "client/assets/sounds/race_start.wav");

        std::cout << "[GAME] All sounds loaded successfully" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[GAME] Error loading sounds: " << e.what() << std::endl;
        std::cerr << "[GAME] The game will continue without audio" << std::endl;
    }

    updateUILayout();
}


bool Game::processFrame(float dt) {
    // Process input
    if (currentState == GameState::RACING) {
        if (!eventHandler.handleEvents())
            return false;  // Window was closed
    } else {
        processInput();
        if (quit)
            return false;
    }

    update(dt);
    render();

    return true;
}


void Game::updateUILayout() {
    int w = window.GetWidth();
    int h = window.GetHeight();
    worldRenderer.updateLayout(w, h);
    interfaceRenderer.updateLayout(w, h);
}


void Game::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = true;
            return;
        }

        if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
            quit = true;
            return;
        }

        if (currentState == GameState::COUNTDOWN)
            continue;

        if (currentState == GameState::MODIFYING_CAR && event.type == SDL_MOUSEBUTTONDOWN) {
            int x = event.button.x;
            int y = event.button.y;

            if (!saved) {
                // VELOCITY BUTTON
                if (interfaceRenderer.getSpeedButtonRect().Contains(x, y)) {
                    speedModified = !speedModified;
                    soundManager.playSound("button_click");
                    std::cout << "[GAME] Velocity "
                              << (speedModified ? "activated" : "deactivated") << std::endl;
                }

                // HEALTH BUTTON
                if (interfaceRenderer.getHealthButtonRect().Contains(x, y)) {
                    healthModified = !healthModified;
                    soundManager.playSound("button_click");
                    std::cout << "[GAME] Health " << (healthModified ? "activated" : "deactivated")
                              << std::endl;
                }
            }

            // SAVE BUTTON
            if (interfaceRenderer.getSaveButtonRect().Contains(x, y) && !saved) {
                saved = true;  // Cannot be undone
                soundManager.playSound("confirm");
                gameLoop.sendModifications(speedModified, healthModified);
                std::cout << "[GAME] ✓ Modifications sent!" << std::endl;
            }
        }
    }
}

GameState Game::getCurrentState() const { return currentState; }

void Game::update(float dt) {
    checkWindowResize();

    worldRenderer.updateEffects(dt / 1000.0f);

    auto carStates = world.getCars();
    raceTimerMs = world.getCountdown();

    if (carStates.count(playerId)) {
        const auto& myCarState = carStates.at(playerId);
        worldRenderer.updateCamera(myCarState.x, myCarState.y);
    }

    if (activeCheatNotification != CheatType::NONE) {
        cheatNotificationTimer -= dt;
        if (cheatNotificationTimer <= 0)
            activeCheatNotification = CheatType::NONE;
    }

    if (eliminatedPopupDelayMs > 0) {
        eliminatedPopupDelayMs -= dt;
        if (eliminatedPopupDelayMs < 0)
            eliminatedPopupDelayMs = 0;
    }
}

void Game::checkWindowResize() {
    int currentWidth = window.GetWidth();
    int currentHeight = window.GetHeight();
    
    // Si cambió el tamaño, actualizar layout
    if (currentWidth != lastWindowWidth || currentHeight != lastWindowHeight) {
        std::cout << "[GAME] Window size changed: " << currentWidth << "x" << currentHeight << std::endl;
        updateUILayout();
        lastWindowWidth = currentWidth;
        lastWindowHeight = currentHeight;
    }
}

void Game::startRace() {
    currentState = GameState::RACING;
    soundManager.playMusic();
}

void Game::setRaceTimer(uint32_t timeMs) { this->raceTimerMs = timeMs; }

void Game::showCountdown(uint8_t number) {
    currentState = GameState::COUNTDOWN;
    countdownNumber = number;
    
    if (number >= 1 && number <= 3)
        soundManager.playSound("countdown");
    else if (number == 0)
        soundManager.playSound("race_start");
}

void Game::showStats(const RaceResults& results) {
    currentState = GameState::SHOWING_STATS;
    currentResults = results;
    statsTimerMs = results.countdownMs;

    soundManager.stopMusic();
    soundManager.playSound("race_end");
}

void Game::showModifications(const CarProperties& props) {
    currentState = GameState::MODIFYING_CAR;
    currentProperties = props;
    modTimerMs = props.countdownMs;
    speedModified = false;
    healthModified = false;
    saved = false;
}

void Game::showCheatNotification(CheatType cheatType) {
    activeCheatNotification = cheatType;
    cheatNotificationTimer = 300.0f;
}

void Game::showFinalResults(const FinalResults& results) {
    currentState = GameState::GAME_END;
    finalResults = results;

    soundManager.stopMusic();
    if (results.winnerId == playerId)
        soundManager.playSound("victory");
}


void Game::onCollision(float x, float y, float intensity) {
    worldRenderer.addCollisionEffect(x, y, intensity);

    // Sound modulated by intensity
    int volume = static_cast<int>(intensity * MIX_MAX_VOLUME);
    soundManager.playSound("collision", volume);

    if (intensity > 0.7f)
        triggerScreenFlash();
}

void Game::onPlayerDied(uint16_t deadPlayerId) {
    auto cars = world.getCars();
    if (cars.count(deadPlayerId)) {
        const auto& deadCar = cars.at(deadPlayerId);

        // Explosion with 50 particles
        worldRenderer.addExplosion(deadCar.x, deadCar.y, 50);
        soundManager.playSound("explosion");

        std::cout << "[GAME] Explosion at (" << deadCar.x << "," << deadCar.y << ")" << std::endl;
    }

    // For the player who died
    if (deadPlayerId == playerId) {
        currentState = GameState::ELIMINATED;
        soundManager.pauseMusic();
        eliminatedPopupDelayMs = 1500.0f;
    }
}

void Game::triggerScreenFlash() {
    screenFlashActive = true;
    flashTimer = 0.2f;  // Lasts 200ms
}


void Game::render() {
    renderer.Clear();
    worldRenderer.render();

    if (screenFlashActive) {
        renderer.SetScale(1.0f, 1.0f);
        renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);

        float alpha = (flashTimer / 0.2f) * 200;  // Fades out
        renderer.SetDrawColor(255, 255, 255, static_cast<Uint8>(alpha));
        renderer.FillRect(Rect(0, 0, window.GetWidth(), window.GetHeight()));
        renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);

        flashTimer -= 0.016f;  // Approx 1 frame at 60fps
        if (flashTimer <= 0)
            screenFlashActive = false;
    }

    switch (currentState) {
        case GameState::COUNTDOWN:
            interfaceRenderer.renderCountdown(countdownNumber);
            break;
        case GameState::RACING:
            interfaceRenderer.renderRaceUI(raceTimerMs, currentRace, totalRaces,
                                              window.GetWidth());
            break;
        case GameState::ELIMINATED:
            if (eliminatedPopupDelayMs <= 0)
                interfaceRenderer.renderEliminatedPopup();
            break;
        case GameState::SHOWING_STATS:
            interfaceRenderer.renderStatsPopup(currentResults, statsTimerMs);
            break;
        case GameState::MODIFYING_CAR:
            interfaceRenderer.renderModificationPopup(speedModified, healthModified, saved,
                                                         modTimerMs, currentProperties);
            break;
        case GameState::GAME_END:
            interfaceRenderer.renderPodium(finalResults);
            break;
    }

    if (currentState == GameState::RACING)
        interfaceRenderer.renderMinimap();

    if (activeCheatNotification != CheatType::NONE)
        interfaceRenderer.renderCheatNotification(activeCheatNotification);

    renderer.Present();
}
