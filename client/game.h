#ifndef GAME_H
#define GAME_H

#include <SDL2pp/SDL.hh>
#include <SDL2pp/Window.hh>
#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>
#include <SDL2pp/SDLTTF.hh> 
#include <SDL2pp/Font.hh>

#include "../common/protocol/game_data.h"
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
    uint8_t player_id; // id del jugador local (este cliente)

    Car player_car;
    bool is_running = true;

    EventHandler event_handler;

    game_state current_state = game_state::MODIFYING_CAR;
    // esto lo envia el servidor
//    float race_timer = 60.0f;          // tiempo de carrera para pruebas (1 min), desp es 600.0f = 10 min
//    float stats_display_timer = 10.0f; // tiempo de la tabla de stats (10 seg)
//    float modification_timer = 10.0f;  // tiempo de la fase de edicion (10 seg)

    uint32_t race_timer_ms = 60000; // 10 min
    uint32_t stats_timer_ms = 10000;
    uint32_t mod_timer_ms = 10000;

    RaceResults current_results;
    CarProperties current_properties;

    // número de carrera actual y total tambien hardocdeado, esa info la envia el sender
    int current_race = 1;
    int total_races = 6;

    // UI responsiva y cámara
    SDL2pp::Rect camera;
    float scale_factor = 1.0f;
    const float REFERENCE_HEIGHT = 600.0f; // alto de referencia para escalar

    // rectangulos de UI (se calculan dinamicamente)
    SDL2pp::Rect speed_button_rect;
    SDL2pp::Rect accel_button_rect;
    SDL2pp::Rect save_button_rect;
    SDL2pp::Rect stats_popup_rect;
    SDL2pp::Rect mod_popup_rect;
    SDL2pp::Rect minimap_rect;

    // hay que ver despues como armamos lo de la modificacions de propiedades

    // flags para simular modificaciones
    bool speed_modified = false;
    bool accel_modified = false;

    void process_input();
    void update(float dt);
    void render();

    void render_map_camera();
    void render_all_cars();
    void render_race_ui();
    void render_stats_popup();
    void render_modification_popup();
    void render_minimap();

public:
    Game(World& world, Client& client, uint8_t player_id);
    void run();
    void start_race();

    // metodos llamados por EventHandler
    game_state get_current_state() const;
    void handle_modification_click(int x, int y);
    void update_ui_layout(); 

    // metodos llamados por Client (desde Receiver)
    void set_race_timer(uint16_t time_ms);
    void show_stats(const RaceResults& results);
    void show_modifications(const CarProperties& props);
};


#endif // GAME_H