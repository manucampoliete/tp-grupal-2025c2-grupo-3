#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <SDL2pp/SDL.hh>
#include <cstdint>
#include "car.h"
#include "game_handler.h"

using namespace SDL2pp;


class Game;
class Client;

class EventHandler {
private:
    GameHandler& game_handler; // ahora referencia al gamehandler en lugar de al client
 //   Car& player_car; // referencia al auto local (para animaciones)
    Game& game;

    bool last_up = false;
    bool last_down = false;
    bool last_left = false;
    bool last_right = false;

    void handle_keyboard();

public:
    EventHandler(GameHandler& game_handler,/* Car& player_car,*/ Game& game);

    // procesa todos los eventos SDL y los reenvía al cliente
    bool handle_events();
};

#endif // EVENT_HANDLER_H
