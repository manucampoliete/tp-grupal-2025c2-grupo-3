#include "event_handler.h"

#include <iostream>

#include <SDL2pp/SDL.hh>

#include "client.h"
#include "event_handler.h"
#include "game.h"

EventHandler::EventHandler(GameHandler& game_handler, Game& game):
        game_handler(game_handler), game(game) {}


bool EventHandler::handle_events() {
    SDL_Event event;
    const Uint8* state = SDL_GetKeyboardState(NULL);

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            return false;
        
        // Manejo de UI responsive
        if (event.type == SDL_WINDOWEVENT) {
            switch (event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                case SDL_WINDOWEVENT_MAXIMIZED:
                case SDL_WINDOWEVENT_RESTORED:
                    game.update_ui_layout();
                    break;
            }
        }

        // CHEATS
        if (event.type == SDL_KEYDOWN) {
            const Uint8* key_state = SDL_GetKeyboardState(NULL);

            switch (event.key.keysym.scancode) {
                // I + M (Trigger: M)
                case SDL_SCANCODE_M:
                    if (key_state[SDL_SCANCODE_I]) {
                        std::cout << "[CHEAT] Inmortalidad (I+M) activada!" << std::endl;
                        game_handler.send_cheat_inmortality();
                    }
                    break;
                    
                // Trigger K
                case SDL_SCANCODE_K:
                    if (key_state[SDL_SCANCODE_I] && key_state[SDL_SCANCODE_W]) {
                        std::cout << "[CHEAT] InstaWin (I+W+K) activado!" << std::endl;
                        game_handler.send_cheat_insta_win();
                    }
                    else if (key_state[SDL_SCANCODE_I] && key_state[SDL_SCANCODE_L]) {
                        std::cout << "[CHEAT] InstaLose (I+L+K) activado!" << std::endl;
                        game_handler.send_cheat_insta_lose();
                    }
                    break;
                
                default:
                    break;
            }
        }
    }

    // MOVIMIENTO
    if (game.get_current_state() == game_state::RACING) {
        bool up = state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP];
        bool down = state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN];
        bool left = state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT];
        bool right = state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT];
        
        if (up != last_up || down != last_down || left != last_left || right != last_right) {
            game_handler.send_movement(up, down, left, right);
            
            last_up = up;
            last_down = down;
            last_left = left;
            last_right = right;
        }
        
    }

    return true;
}