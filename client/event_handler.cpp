#include "event_handler.h"
#include <SDL2pp/SDL.hh>
#include <iostream>

EventHandler::EventHandler(Client& client, Car& player_car)
    : client(client), player_car(player_car) {}

bool EventHandler::handle_events() {
    SDL_Event event;
    const Uint8* state = SDL_GetKeyboardState(NULL);

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            return false;

        // se envía el estado de las teclas en cada frame, sin importar si hubo un evento de teclado o no
        // esto captura el estado actual de las teclas presionadas
        // esto no es lo ideal, lo uso para probar ahora
        // despues voy a implementar el del cambio de estado

        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
            player_car.handle_event(event);

    }

    bool up = state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP];
    bool down = state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN];
    bool left = state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT];
    bool right = state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT];
    
    client.send_movement(up, down, left, right);

    return true;
    //}
}
