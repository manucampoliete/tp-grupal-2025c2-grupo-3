#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <cstdint>
#include <SDL2pp/SDL.hh>

using namespace SDL2pp;  // NOLINT

class Game;
class GameLoop;

class EventHandler {
private:
    GameLoop& gameLoop;
    Game& game;

    bool lastUp = false;
    bool lastDown = false;
    bool lastLeft = false;
    bool lastRight = false;

public:
    EventHandler(GameLoop& gameLoop, Game& game);
    
    // Processes all SDL events and forwards them to the client
    bool handleEvents();
};

#endif  // EVENT_HANDLER_H
