#ifndef GAME_H
#define GAME_H

#include <SDL2pp/SDL.hh>
#include <SDL2pp/Window.hh>
#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>
#include <SDL2pp/SDLTTF.hh> 
#include <SDL2pp/Font.hh>

#include "car.h"
#include "world.h"
#include "client.h"
#include "event_handler.h"


using namespace SDL2pp;

enum class game_state {
    RACING,
    SHOWING_STATS,
    MODIFYING_CAR
};

class Game {
private:
    SDL2pp::SDL sdl;
    SDL2pp::SDLTTF ttf;
    SDL2pp::Window window;
    SDL2pp::Renderer renderer;
    SDL2pp::Font font;
    
    SDL2pp::Texture map_texture;
    SDL2pp::Texture car_sprites;

    World& world; // referencia al mundo (todos los autos)
    Client& client; // referencia al cliente (para enviar inputs)
    uint8_t player_id = 0; // id del jugador local (este cliente)

    Car player_car;
    bool is_running = true;

    EventHandler event_handler;

    game_state current_state = game_state::RACING;
    float race_timer = 60.0f;          // tiempo de carrera para pruebas (1 min), desp es 600.0f = 10 min
    float stats_display_timer = 10.0f; // tiempo de la tabla de stats (10 seg)
    float modification_timer = 10.0f;  // tiempo de la fase de edicion (10 seg)

    // número de carrera actual y total tambien hardocdeado, esa info la envia el sender
    int current_race = 1;
    int total_races = 6;

    // botones simulados en la fase de modificación
    // hay que ver despues como armamos lo de la modificacions de propiedades
    SDL2pp::Rect speed_button_rect;
    SDL2pp::Rect accel_button_rect;
    SDL2pp::Rect save_button_rect;

    // flags para simular modificaciones
    bool speed_modified = false;
    bool accel_modified = false;

    void process_input();
    void update(float dt);

    void start_race();
    void show_stats();
    void show_modifications();

    void render_all_cars();

    void render_race_ui();
    void render_stats_popup();
    void render_modification_popup();
    void render();

public:
    Game(World& world, Client& client, uint8_t player_id);
    void run();
};


#endif // GAME_H