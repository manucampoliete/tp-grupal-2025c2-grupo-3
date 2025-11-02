#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <SDL2pp/SDL.hh>
#include <cstdint>
#include "client.h"
#include "car.h"

using namespace SDL2pp;

class Game;

class EventHandler {
private:
    Client& client;  // referencia al cliente para enviar inputs al servidor
    Car& player_car; // referencia al auto local (para animaciones)
    Game& game;

    bool last_up = false;
    bool last_down = false;
    bool last_left = false;
    bool last_right = false;

    void handle_keyboard();

public:
    EventHandler(Client& client, Car& player_car, Game& game);

    // procesa todos los eventos SDL y los reenvía al cliente
    bool handle_events();
};

#endif // EVENT_HANDLER_H
