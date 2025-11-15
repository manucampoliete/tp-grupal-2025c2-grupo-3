#include "event_handler.h"

#include <iostream>
#include <SDL2pp/SDL.hh>

#include "game.h"
#include "game_loop.h"

EventHandler::EventHandler(GameLoop& game_loop, Game& game):
        game_loop(game_loop), game(game) {}


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

        // CHEATS Y SONIDOS
        if (event.type == SDL_KEYDOWN) {
            const Uint8* key_state = SDL_GetKeyboardState(NULL);

            switch (event.key.keysym.scancode) {
                // I + L (Trigger: L)
                case SDL_SCANCODE_L:
                    if (key_state[SDL_SCANCODE_I]) {
                        std::cout << "[CHEAT] Inmortalidad (I+L) activada!" << std::endl;
                        game_loop.send_cheat_inmortality();
                    }
                    break;
                    
                // Trigger K
                case SDL_SCANCODE_K:
                    if (key_state[SDL_SCANCODE_I] && key_state[SDL_SCANCODE_W]) {
                        std::cout << "[CHEAT] InstaWin (I+W+K) activado!" << std::endl;
                        game_loop.send_cheat_insta_win();
                    }
                    else if (key_state[SDL_SCANCODE_I] && key_state[SDL_SCANCODE_L]) {
                        std::cout << "[CHEAT] InstaLose (I+L+K) activado!" << std::endl;
                        game_loop.send_cheat_insta_lose();
                    }
                    break;
            
                case SDL_SCANCODE_EQUALS: 
                case SDL_SCANCODE_KP_PLUS:
                    if (key_state[SDL_SCANCODE_LCTRL] || key_state[SDL_SCANCODE_RCTRL]) {
                        // Ctrl + '+': subir vol de musica
                        int current_vol = game.get_sound_manager().is_music_enabled() ? 64 : 0;
                        game.get_sound_manager().set_music_volume(std::min(current_vol + 10, 128));
                        std::cout << "[SOUND] 🔊 Volumen de música aumentado" << std::endl;
                    }
                    break;

                case SDL_SCANCODE_MINUS:
                case SDL_SCANCODE_KP_MINUS:
                    if (key_state[SDL_SCANCODE_LCTRL] || key_state[SDL_SCANCODE_RCTRL]) {
                        // Ctrl + '-': bajar vol de musica
                        int current_vol = 64;
                        game.get_sound_manager().set_music_volume(std::max(current_vol - 10, 0));
                        std::cout << "[SOUND] 🔉 Volumen de música reducido" << std::endl;
                    }
                    break;

                case SDL_SCANCODE_M:
                    if (key_state[SDL_SCANCODE_LCTRL] || key_state[SDL_SCANCODE_RCTRL]) {
                        // Ctrl + M: mutear/desmutear musica
                        game.get_sound_manager().toggle_music();
                    }
                    break;

                case SDL_SCANCODE_N:
                    if (key_state[SDL_SCANCODE_LCTRL] || key_state[SDL_SCANCODE_RCTRL]) {
                        // Ctrl + N: mutear/desmutear sonidos
                        game.get_sound_manager().toggle_sfx();
                    }
                    break;
                
                case SDL_SCANCODE_ESCAPE:
                    std::cout << "[GAME] ESC presionado, cerrando juego..." << std::endl;
                    return false;  // cerrar juego
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
            game_loop.send_movement(up, down, left, right);
            
            last_up = up;
            last_down = down;
            last_left = left;
            last_right = right;
        }
        
    }

    return true;
}