#ifndef GAME_H
#define GAME_H

#include <SDL2pp/Font.hh>
#include <SDL2pp/Renderer.hh>
#include <SDL2pp/SDL.hh>
#include <SDL2pp/SDLTTF.hh>
#include <SDL2pp/Texture.hh>
#include <SDL2pp/Window.hh>

#include "../common/messages/game_data.h"
#include "../common/messages/snapshot.h"

#include "event_handler.h"
#include "interface_renderer.h"
#include "sound_manager.h"
#include "world.h"
#include "world_renderer.h"

using namespace SDL2pp;  // NOLINT


class GameLoop;


enum class game_state { COUNTDOWN, RACING, ELIMINATED, SHOWING_STATS, MODIFYING_CAR, GAME_END };


class Game {
private:
    SDL2pp::SDL sdl;
    SDL2pp::SDLTTF ttf;
    SDL2pp::Window window;
    SDL2pp::Renderer renderer;
    SDL2pp::Font font;
    SDL2pp::Font font_small;
    SDL2pp::Font font_big;

    SDL2pp::Texture map_texture;
    SDL2pp::Texture car_sprites;

    SDL2pp::Texture cheat_inmortality_img;
    SDL2pp::Texture cheat_win_img;
    SDL2pp::Texture cheat_lose_img;

    World& world;
    GameLoop& game_loop;
    uint8_t player_id;

    bool quit = false;

    EventHandler event_handler;

    WorldRenderer world_renderer;
    UIRenderer interface_renderer;
    SoundManager sound_manager;

    game_state current_state =
            game_state::RACING;  // va COUNTDOWN pero hasta arreglarlo asi se ve la carrera

    // countdown
    uint8_t countdown_number = 3;  // 3, 2, 1, 0=GO
    float countdown_timer = 0.0f;  // timer interno para cambiar numeros

    uint32_t race_timer_ms;
    uint32_t stats_timer_ms;
    uint32_t mod_timer_ms;

    RaceResults current_results;
    CarProperties current_properties;
    FinalResults final_results;

    // número de carrera actual y total hardocdeado, esa info la envia el server
    int current_race = 1;
    int total_races = 6;

    bool speed_modified = false;
    bool health_modified = false;
    bool saved = false;

    // efectos visuales
    bool screen_flash_active = false;
    float flash_timer = 0.0f;

    CheatType active_cheat_notification = CheatType::NONE;
    float cheat_notification_timer = 0.0f;

    float eliminated_popup_delay_ms;

    void process_input();
    void update(float dt);
    void render();

public:
    Game(World& world, GameLoop& game_loop, uint8_t player_id);

    /*
     * procesa un frame completo (input, update, render)
     * devuelve false si debe cerrar el juego
     */
    bool process_frame(float dt);

    void start_race();

    bool should_close() const { return quit; }

    game_state get_current_state() const;
    void update_ui_layout();

    void set_race_timer(uint16_t time_ms);
    void show_countdown(uint8_t number);
    void show_stats(const RaceResults& results);
    void show_modifications(const CarProperties& props);
    void show_cheat_notification(CheatType cheat_type);
    void show_final_results(const FinalResults& results);

    void on_player_died(uint16_t player_id);
    void on_collision(float x, float y, float intensity);
    void trigger_screen_flash();

    SoundManager& get_sound_manager() { return sound_manager; }
    WorldRenderer& get_world_renderer() { return world_renderer; }
    const CarProperties& get_current_properties() const { return current_properties; }
};

#endif  // GAME_H
