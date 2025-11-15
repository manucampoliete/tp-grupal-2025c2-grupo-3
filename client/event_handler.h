#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <cstdint>

#include <SDL2pp/SDL.hh>


using namespace SDL2pp;  // NOLINT


class Game;
class GameLoop;


class EventHandler {
private:
    GameLoop& game_loop;
    Game& game;

    bool last_up = false;
    bool last_down = false;
    bool last_left = false;
    bool last_right = false;

public:
    EventHandler(GameLoop& game_loop, Game& game);

    // procesa todos los eventos SDL y los reenvía al cliente
    bool handle_events();
};

#endif  // EVENT_HANDLER_H
