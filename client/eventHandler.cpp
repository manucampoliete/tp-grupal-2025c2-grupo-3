#include "eventHandler.h"

#include <algorithm>
#include <iostream>

#include <SDL2pp/SDL.hh>

#include "game.h"
#include "gameLoop.h"

EventHandler::EventHandler(GameLoop& gameLoop, Game& game): gameLoop(gameLoop), game(game) {}


bool EventHandler::handleEvents() {
    SDL_Event event;
    const Uint8* state = SDL_GetKeyboardState(NULL);

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            return false;
        // Responsive UI handling
        if (event.type == SDL_WINDOWEVENT) {
            switch (event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                case SDL_WINDOWEVENT_MAXIMIZED:
                case SDL_WINDOWEVENT_RESTORED:
                    game.updateUILayout();
                    break;
            }
        }

        // CHEATS AND SOUNDS
        if (event.type == SDL_KEYDOWN) {
            const Uint8* keyState = SDL_GetKeyboardState(NULL);

            switch (event.key.keysym.scancode) {
                // I + L (Trigger: L)
                case SDL_SCANCODE_L:
                    if (keyState[SDL_SCANCODE_I]) {
                        std::cout << "[CHEAT] Inmortality (I+L) activated!" << std::endl;
                        gameLoop.sendCheatInmortality();
                    }
                    break;

                // G + C (Trigger: C)
                case SDL_SCANCODE_C:
                    if (keyState[SDL_SCANCODE_G]) {
                        std::cout << "[CHEAT] InstaWin (G+C) activated!" << std::endl;
                        gameLoop.sendCheatInstaWin();
                    }
                    break;

                // L + E (Trigger: E)
                case SDL_SCANCODE_E:
                    if (keyState[SDL_SCANCODE_L]) {
                        std::cout << "[CHEAT] InstaLose (L+E) activated!" << std::endl;
                        gameLoop.sendCheatInstaLose();
                    }
                    break;

                case SDL_SCANCODE_EQUALS:
                case SDL_SCANCODE_KP_PLUS:
                    if (keyState[SDL_SCANCODE_LCTRL] || keyState[SDL_SCANCODE_RCTRL]) {
                        // Ctrl + '+': increase music volume
                        int currentVol = game.getSoundManager().isMusicEnabled() ? 64 : 0;
                        game.getSoundManager().setMusicVolume(std::min(currentVol + 10, 128));
                        std::cout << "[SOUND] 🔊 Music volume increased" << std::endl;
                    }
                    break;

                case SDL_SCANCODE_MINUS:
                case SDL_SCANCODE_KP_MINUS:
                    if (keyState[SDL_SCANCODE_LCTRL] || keyState[SDL_SCANCODE_RCTRL]) {
                        // Ctrl + '-': decrease music volume
                        int currentVol = 64;
                        game.getSoundManager().setMusicVolume(std::max(currentVol - 10, 0));
                        std::cout << "[SOUND] 🔉 Music volume decreased" << std::endl;
                    }
                    break;

                case SDL_SCANCODE_M:
                    if (keyState[SDL_SCANCODE_LCTRL] || keyState[SDL_SCANCODE_RCTRL]) {
                        // Ctrl + M: mute/unmute music
                        game.getSoundManager().toggleMusic();
                    }
                    break;

                case SDL_SCANCODE_N:
                    if (keyState[SDL_SCANCODE_LCTRL] || keyState[SDL_SCANCODE_RCTRL]) {
                        // Ctrl + N: mute/unmute sounds
                        game.getSoundManager().toggleSfx();
                    }
                    break;

                case SDL_SCANCODE_ESCAPE:
                    std::cout << "[GAME] ESC pressed, closing game..." << std::endl;
                    return false;  // Close game
                    break;

                default:
                    break;
            }
        }
    }

    // MOVEMENT
    if (game.getCurrentState() == GameState::RACING) {
        bool up = state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP];
        bool down = state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN];
        bool left = state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT];
        bool right = state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT];

        // Only send if there's a change
        if (up != lastUp || down != lastDown || left != lastLeft || right != lastRight) {
            gameLoop.sendMovement(up, down, left, right);

            // Update last state
            lastUp = up;
            lastDown = down;
            lastLeft = left;
            lastRight = right;
        }
    }

    return true;
}
