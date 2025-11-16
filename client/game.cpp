#include "game.h"

#include <SDL2pp/SDL.hh>
#include <SDL2pp/SDL2pp.hh>

#include "game_loop.h"


Game::Game(World& world, GameLoop& game_loop, uint8_t player_id):
        sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO),
        ttf(),
        window("Need For Speed", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,
               SDL_WINDOW_RESIZABLE),
        renderer(window, -1, SDL_RENDERER_ACCELERATED),
        font("client/assets/fonts/VCR_OSD_MONO.ttf", 24),
        font_small("client/assets/fonts/VCR_OSD_MONO.ttf", 18),
        font_big("client/assets/fonts/VCR_OSD_MONO.ttf", 30),

        // cargo la textura del mapa desde un archivo
        // por ahora hardcodeo uno
        // cambiar cunaod este el editor listo
        map_texture(renderer, "client/assets/cities/Game Boy _ GBC - Grand Theft Auto - "
                              "Backgrounds - Vice City.png"),

        // cargo la textura con los sprites de autos
        car_sprites(
                renderer,
                SDL2pp::Surface(
                        "client/assets/cars/Mobile - Grand Theft Auto 4 - Miscellaneous - Cars.png")
                        .SetColorKey(true, 0xa3a30d)),

        cheat_inmortality_img(renderer, "client/assets/cheats/inmortality.png"),
        cheat_win_img(renderer, "client/assets/cheats/win.png"),
        cheat_lose_img(renderer, "client/assets/cheats/lose.png"),

        world(world),
        game_loop(game_loop),
        player_id(player_id),
        event_handler(game_loop, *this),
        world_renderer(renderer, map_texture, car_sprites, world, player_id),
        interface_renderer(renderer, font, font_small, font_big, map_texture, world, player_id,
                           cheat_inmortality_img, cheat_win_img, cheat_lose_img),
        sound_manager() {

    eliminated_popup_delay_ms = 0.0f;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_SetHint(SDL_HINT_AUDIO_RESAMPLING_MODE, "1");
    SDL_SetWindowMinimumSize(window.Get(), 800, 600);

    try {
        sound_manager.load_music("client/assets/sounds/music.mp3");
        sound_manager.load_sound("collision", "client/assets/sounds/crash.wav");
        sound_manager.load_sound("explosion", "client/assets/sounds/explosion.wav");
        sound_manager.load_sound("checkpoint", "client/assets/sounds/checkpoint.wav");
        sound_manager.load_sound("countdown", "client/assets/sounds/beep.wav");
        sound_manager.load_sound("race_end", "client/assets/sounds/finish.wav");
        sound_manager.load_sound("brake", "client/assets/sounds/brake.wav");
        sound_manager.load_sound("engine", "client/assets/sounds/engine.wav");
        sound_manager.load_sound("confirm", "client/assets/sounds/confirm.wav");
        sound_manager.load_sound("victory", "client/assets/sounds/victory.wav");

        std::cout << "[GAME] Todos los sonidos cargados correctamente" << std::endl;

        // a chequear cuando este el flujo completo!
        // esto por ahi no tendria que ir aca cuando este el countdown?
        sound_manager.play_music();

    } catch (const std::exception& e) {
        std::cerr << "[GAME] Error cargando sonidos: " << e.what() << std::endl;
        std::cerr << "[GAME] El juego continuará sin audio" << std::endl;
    }

    update_ui_layout();
}


bool Game::process_frame(float dt) {
    // procesar input
    if (current_state == game_state::RACING) {
        if (!event_handler.handle_events())
            return false;  // se cerró la ventana
    } else {
        process_input();
        if (quit)
            return false;
    }

    update(dt);
    render();

    return true;
}


void Game::update_ui_layout() {
    int w = window.GetWidth();
    int h = window.GetHeight();
    world_renderer.update_layout(w, h);
    interface_renderer.update_layout(w, h);
}


void Game::process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = true;
            return;
        }

        if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
            quit = true;
            return;
        }

        if (current_state == game_state::COUNTDOWN)
            continue;

        if (current_state == game_state::MODIFYING_CAR && event.type == SDL_MOUSEBUTTONDOWN) {
            int x = event.button.x;
            int y = event.button.y;

            if (!saved) {
                // BOTÓN DE VELOCIDAD
                if (interface_renderer.get_speed_button_rect().Contains(x, y)) {
                    speed_modified = !speed_modified;
                    sound_manager.play_sound("button_click");
                    std::cout << "[GAME] Velocidad "
                              << (speed_modified ? "activada" : "desactivada") << std::endl;
                }

                // BOTÓN DE SALUD
                if (interface_renderer.get_health_button_rect().Contains(x, y)) {
                    health_modified = !health_modified;
                    sound_manager.play_sound("button_click");
                    std::cout << "[GAME] Salud " << (health_modified ? "activada" : "desactivada")
                              << std::endl;
                }
            }

            // BOTÓN GUARDAR
            if (interface_renderer.get_save_button_rect().Contains(x, y) && !saved) {
                saved = true;  // no se puede deshacer
                sound_manager.play_sound("confirm");
                game_loop.send_modifications(speed_modified, health_modified);
                std::cout << "[GAME] ✓ Modificaciones enviadas!" << std::endl;
            }
        }
    }
}

game_state Game::get_current_state() const { return current_state; }

void Game::update(float dt) {
    world_renderer.update_effects(dt / 1000.0f);

    auto car_states = world.getCars();
    race_timer_ms = world.getCountdown();

    if (car_states.count(player_id)) {
        const auto& my_car_state = car_states.at(player_id);
        world_renderer.update_camera(my_car_state.x, my_car_state.y);
    }

    if (active_cheat_notification != CheatType::NONE) {
        cheat_notification_timer -= dt;
        if (cheat_notification_timer <= 0)
            active_cheat_notification = CheatType::NONE;
    }

    if (eliminated_popup_delay_ms > 0) {
        eliminated_popup_delay_ms -= dt;
        if (eliminated_popup_delay_ms < 0)
            eliminated_popup_delay_ms = 0;
    }
}


void Game::start_race() {
    current_state = game_state::RACING;
    // sonido de inicio (diferente al countdown)
    // sound_manager.play_sound("race_start");
}

void Game::set_race_timer(uint16_t time_ms) { this->race_timer_ms = time_ms; }

void Game::show_countdown(uint8_t number) {
    current_state = game_state::COUNTDOWN;
    countdown_number = number;
    countdown_timer = 0.0f;
    if (number <= 3)
        sound_manager.play_sound("countdown");
    // En GO! desp veo si usar game_start o directamente la musica
}

void Game::show_stats(const RaceResults& results) {
    current_state = game_state::SHOWING_STATS;
    current_results = results;
    stats_timer_ms = results.countdown_ms;

    sound_manager.stop_music();
    sound_manager.play_sound("race_end");
}

void Game::show_modifications(const CarProperties& props) {
    current_state = game_state::MODIFYING_CAR;
    current_properties = props;
    mod_timer_ms = props.countdown_ms;
    speed_modified = false;
    health_modified = false;
    saved = false;
}

void Game::show_cheat_notification(CheatType cheat_type) {
    active_cheat_notification = cheat_type;
    cheat_notification_timer = 300.0f;
}

void Game::show_final_results(const FinalResults& results) {
    current_state = game_state::GAME_END;
    final_results = results;

    sound_manager.stop_music();
    if (results.winner_id == player_id)
        sound_manager.play_sound("victory");
}


void Game::on_collision(float x, float y, float intensity) {
    world_renderer.add_collision_effect(x, y, intensity);

    // conido modulado por intensidad
    int volume = static_cast<int>(intensity * MIX_MAX_VOLUME);
    sound_manager.play_sound("collision", volume);

    if (intensity > 0.7f)
        trigger_screen_flash();
}

void Game::on_player_died(uint16_t dead_player_id) {
    auto cars = world.getCars();
    if (cars.count(dead_player_id)) {
        const auto& dead_car = cars.at(dead_player_id);

        // explosion con 50 partculas
        world_renderer.add_explosion(dead_car.x, dead_car.y, 50);
        sound_manager.play_sound("explosion");

        std::cout << "[GAME] Explosión en (" << dead_car.x << "," << dead_car.y << ")" << std::endl;
    }

    // para el player que murio
    if (dead_player_id == player_id) {
        current_state = game_state::ELIMINATED;
        sound_manager.pause_music();
        eliminated_popup_delay_ms = 1500.0f;
    }
}

void Game::trigger_screen_flash() {
    screen_flash_active = true;
    flash_timer = 0.2f;  // dura 200ms
}


void Game::render() {
    renderer.Clear();
    world_renderer.render();

    if (screen_flash_active) {
        renderer.SetScale(1.0f, 1.0f);
        renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);

        float alpha = (flash_timer / 0.2f) * 200;  // se desvanece
        renderer.SetDrawColor(255, 255, 255, static_cast<Uint8>(alpha));
        renderer.FillRect(Rect(0, 0, window.GetWidth(), window.GetHeight()));
        renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);

        flash_timer -= 0.016f;  // aprox 1 frame a 60fps
        if (flash_timer <= 0)
            screen_flash_active = false;
    }

    switch (current_state) {
        case game_state::COUNTDOWN:
            interface_renderer.render_countdown(countdown_number);
            break;
        case game_state::RACING:
            interface_renderer.render_race_ui(race_timer_ms, current_race, total_races,
                                              window.GetWidth());
            break;
        case game_state::ELIMINATED:
            if (eliminated_popup_delay_ms <= 0)
                interface_renderer.render_eliminated_popup();
            break;
        case game_state::SHOWING_STATS:
            interface_renderer.render_stats_popup(current_results, stats_timer_ms);
            break;
        case game_state::MODIFYING_CAR:
            interface_renderer.render_modification_popup(speed_modified, health_modified, saved,
                                                         mod_timer_ms, current_properties);
            break;
        case game_state::GAME_END:
            interface_renderer.render_podium(final_results);
            break;
    }

    if (current_state == game_state::RACING)
        interface_renderer.render_minimap();

    if (active_cheat_notification != CheatType::NONE)
        interface_renderer.render_cheat_notif(active_cheat_notification);

    renderer.Present();
}
