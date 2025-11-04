#include "event_handler.h"
#include <SDL2pp/SDL.hh>
#include <iostream>
#include "game.h"

EventHandler::EventHandler(Client& client, Car& player_car, Game& game)
    : client(client), player_car(player_car), game(game) {}


/**
 * VERSION 1
 * detecta el cambio de estado en el teclado en lugar de cada vez que se aprite la tecla
 * problema: si mantengo apretada la flechita no se mueve constantemente como deberia
 * arreglo: els erver esta esperando updates ctes y no por cambio de estado
 */
/*
bool EventHandler::handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            return false;
    }

    // leo el estado actual del teclado una sola vez por frame
    const Uint8* state = SDL_GetKeyboardState(NULL);
    bool up = state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP];
    bool down = state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN];
    bool left = state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT];
    bool right = state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT];

    // comparo el estado actual con el anterior
    if (up != last_up || down != last_down || left != last_left || right != last_right) {
        // si hubo un cambio envio el nuevo estado
        client.send_movement(up, down, left, right);

        // y actualizo el estado anterior
        last_up = up;
        last_down = down;
        last_left = left;
        last_right = right;
    }

    return true;
}
*/


/**
 * VERSION 2
 * si mantengo apretada la tecla se mueve hasta wue lo suelte
 * contra: envia el estado en cada frame, sin importar si hubo un evento de teclado o no
 */
bool EventHandler::handle_events() {
    SDL_Event event;
    const Uint8* state = SDL_GetKeyboardState(NULL);

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            return false;
        
        // manejo de ui responsive
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
    }

    if (game.get_current_state() == game_state::RACING) {
        bool last_up = state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP];
        bool last_down = state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN];
        bool last_left = state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT];
        bool last_right = state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT];
        
        client.send_movement(last_up, last_down, last_left, last_right);
    }

    return true;
}
