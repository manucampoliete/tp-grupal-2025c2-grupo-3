#ifndef GAME_H
#define GAME_H

#include <SDL2pp/SDL.hh>
#include <SDL2pp/Window.hh>
#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>
#include <SDL2pp/SDLTTF.hh> 
#include <SDL2pp/Font.hh>

#include "../common/messages/game_data.h"
#include "../common/messages/snapshot.h"
#include "car.h"
#include "world.h"
#include "event_handler.h"
#include "world_renderer.h"
#include "interface_renderer.h"

using namespace SDL2pp;


class Client;

enum class game_state {
    COUNTDOWN,
    RACING,
    ELIMINATED,
    SHOWING_STATS,
    MODIFYING_CAR,
    GAME_END
};

class Game {
private:
    SDL2pp::SDL sdl;
    SDL2pp::SDLTTF ttf;
    SDL2pp::Window window;
    SDL2pp::Renderer renderer;
    SDL2pp::Font font;
    SDL2pp::Font font_small;
    
    SDL2pp::Texture map_texture;
    SDL2pp::Texture car_sprites;

    World& world;
    GameHandler& game_handler;
    uint8_t player_id;

 //   Car player_car;
    bool is_running = true;

    EventHandler event_handler;

    // Renderers
    WorldRenderer world_renderer;
    UIRenderer interface_renderer;

    game_state current_state = game_state::COUNTDOWN;

    // countdown
    uint8_t countdown_number = 3; // 3, 2, 1, 0=GO
    float countdown_timer = 0.0f; // timer interno para cambiar numeros

    // esto lo envia el servidor
    uint32_t race_timer_ms = 20000; // 20 seg
    uint32_t stats_timer_ms = 10000;
    uint32_t mod_timer_ms = 10000;

    RaceResults current_results;
    CarProperties current_properties;

    // número de carrera actual y total tambien hardocdeado, esa info la envia el sender
    int current_race = 1;
    int total_races = 6;

    // hay que ver despues como armamos lo de la modificacions de propiedades
    // flags para simular modificaciones
    bool speed_modified = false;
    bool health_modified = false;
    bool saved = false;

    void process_input();
    void update(float dt);
    void render();

public:
    Game(World& world, GameHandler& game_handler, uint8_t player_id);
    void run();
    void start_race();

    // metodos llamados por EventHandler
    game_state get_current_state() const;
    void handle_modification_click(int x, int y);
    void update_ui_layout(); 

    // metodos llamados por Client (desde Receiver)
    void set_race_timer(uint16_t time_ms);
    void show_countdown(uint8_t number);
    void show_stats(const RaceResults& results);
    void show_modifications(const CarProperties& props);
};

#endif // GAME_H