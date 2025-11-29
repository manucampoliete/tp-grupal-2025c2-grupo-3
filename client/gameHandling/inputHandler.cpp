#include "inputHandler.h"

#include <iostream>

#include <SDL2pp/SDL.hh>

#include "../threads/gameLoop.h"
#include "gameStateManager.h"
#include "../audio/soundManager.h"
#include "../rendering/worldRenderer.h"
#include "../rendering/interfaceRenderer.h"


InputHandler::InputHandler(GameLoop& gameLoop, GameStateManager& stateManager, SoundManager& soundManager, 
                        WorldRenderer& worldRenderer, UIRenderer& uiRenderer) :
    gameLoop(gameLoop),
    stateManager(stateManager),
    soundManager(soundManager),
    worldRenderer(worldRenderer),
    uiRenderer(uiRenderer) {}


bool InputHandler::processEvents(bool& windowResized) {
    SDL_Event event;
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    windowResized = false;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            return false;

        // Window events
        if (event.type == SDL_WINDOWEVENT) {
            switch (event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                case SDL_WINDOWEVENT_MAXIMIZED:
                case SDL_WINDOWEVENT_RESTORED:
                case SDL_WINDOWEVENT_SHOWN:
                case SDL_WINDOWEVENT_EXPOSED:
                    windowResized = true;
                    break;
            }
        }

        // Key down events
        if (event.type == SDL_KEYDOWN) {
            handleKeyDown(event, keyState);
            if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                return false;
        }

        // Mouse click (for mod menu)
        if (event.type == SDL_MOUSEBUTTONDOWN)
            handleMouseClick(event);
    }

    // Continuous movement input (only during racing)
    if (stateManager.getState() == GameState::RACING) {
        handleMovement(keyState);
    } else {
        // Not racing, stop movement effects
        worldRenderer.setAccelerating(false);
        worldRenderer.setBraking(false);
        soundManager.stopEngineLoop();
        soundManager.stopBraking();
    }

    return true;
}


void InputHandler::handleKeyDown(const SDL_Event& event, const Uint8* keyState) {
    SDL_Scancode key = event.key.keysym.scancode;

    // Cheats (only during racing)
    if (stateManager.getState() == GameState::RACING)
        handleCheats(key, keyState);

    // Sound controls (always)
    handleSoundControls(key, keyState);
}


void InputHandler::handleCheats(SDL_Scancode key, const Uint8* keyState) {
    // I + L (Trigger L) = Immortality
    if (key == SDL_SCANCODE_L && keyState[SDL_SCANCODE_I])
        gameLoop.sendCheatInmortality();

    // G + C (Trigger C) = Instant Win
    if (key == SDL_SCANCODE_C && keyState[SDL_SCANCODE_G])
        gameLoop.sendCheatInstaWin();

    // L + E (Trigger E) = Instant Lose
    if (key == SDL_SCANCODE_E && keyState[SDL_SCANCODE_L])
        gameLoop.sendCheatInstaLose();
    
    // F + H (Trigger H) = Super Speed
    if (key == SDL_SCANCODE_H && keyState[SDL_SCANCODE_F])
        gameLoop.sendCheatSuperSpeed();
}


void InputHandler::handleSoundControls(SDL_Scancode key, const Uint8* keyState) {
    bool ctrl = keyState[SDL_SCANCODE_LCTRL] || keyState[SDL_SCANCODE_RCTRL];
    if (!ctrl) return;

    if (key == SDL_SCANCODE_M)
        soundManager.toggleMusic();

    if (key == SDL_SCANCODE_N)
        soundManager.toggleSfx();

    if (key == SDL_SCANCODE_EQUALS || key == SDL_SCANCODE_KP_PLUS) {
        int vol = soundManager.isMusicEnabled() ? 64 : 0;
        soundManager.setMusicVolume(std::min(vol + 10, 128));
    }

    if (key == SDL_SCANCODE_MINUS || key == SDL_SCANCODE_KP_MINUS)
        soundManager.setMusicVolume(std::max(64 - 10, 0));
}


void InputHandler::handleMouseClick(const SDL_Event& event) {
    if (stateManager.getState() != GameState::MODIFYING_CAR)
        return;

    int x = event.button.x;
    int y = event.button.y;

    if (!stateManager.areModsSaved()) {
        // Speed button
        if (uiRenderer.getSpeedButtonRect().Contains(x, y)) {
            stateManager.toggleSpeedMod();
            soundManager.playSound("button_click");
        }

        // Health button
        if (uiRenderer.getHealthButtonRect().Contains(x, y)) {
            stateManager.toggleHealthMod();
            soundManager.playSound("button_click");
        }

        // Accel button
        if (uiRenderer.getAccelButtonRect().Contains(x, y)) {
            stateManager.toggleAccelMod();
            soundManager.playSound("button_click");
        }

        // Mass button
        if (uiRenderer.getMassButtonRect().Contains(x, y)) {
            stateManager.toggleMassMod();
            soundManager.playSound("button_click");
        }
    }

    // Save button
    if (uiRenderer.getSaveButtonRect().Contains(x, y) && !stateManager.areModsSaved()) {
        stateManager.saveMods();
        soundManager.playSound("confirm");
        gameLoop.sendModifications(stateManager.isSpeedModified(), stateManager.isHealthModified(),
                                    stateManager.isAccelModified(), stateManager.isMassModified());
        std::cout << "[INPUT] Modifications saved and sent" << std::endl;
    }
}


void InputHandler::handleMovement(const Uint8* state) {
    bool up = state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP];
    bool down = state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN];
    bool left = state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT];
    bool right = state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT];

    // Ony send if changed
    if (up != lastUp || down != lastDown || left != lastLeft || right != lastRight) {
        gameLoop.sendMovement(up, down, left, right);
        lastUp = up;
        lastDown = down;
        lastLeft = left;
        lastRight = right;
    }

    // effects for acceleration
    if (up && !down) {
        worldRenderer.setAccelerating(true);
        soundManager.playEngineLoop();
    } else {
        worldRenderer.setAccelerating(false);
        soundManager.stopEngineLoop();
    }

    // effects for braking
    if (down && !up) {
        worldRenderer.setBraking(true);
        soundManager.playBrakeSound();
    } else {
        worldRenderer.setBraking(false);
        soundManager.stopBraking();
    }
}