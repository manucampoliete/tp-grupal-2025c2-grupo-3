#include "game.h"
#include "game_loop.h"

#include <SDL2pp/SDL.hh>
#include <SDL2pp/SDL2pp.hh>


// muestro todo lo que pasa en el juego
// por ahi mas adelante cuando haya mas cosas convenga separar en mas clases


Game::Game(World& world, GameLoop& game_loop, uint8_t player_id):
        sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO),
        ttf(),

        // ventana principal con título, pos automática y tamaño 800x600
        window("Need For Speed", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,
               SDL_WINDOW_RESIZABLE),

        // renderer acelerado por hardware
        renderer(window, -1, SDL_RENDERER_ACCELERATED),
        font("client/assets/fonts/VCR_OSD_MONO.ttf", 24),  // font
        font_small("client/assets/fonts/VCR_OSD_MONO.ttf", 18),
        font_big("client/assets/fonts/VCR_OSD_MONO.ttf", 150),

        // cargo la textura del mapa desde un archivo
        // por ahora hardcodeo una cualquiera
        map_texture(renderer, "client/assets/cities/Game Boy _ GBC - Grand Theft Auto - "
                              "Backgrounds - Vice City.png"),

        // cargo la textura con los sprites de autos
        car_sprites(
                renderer,
                SDL2pp::Surface(
                        "client/assets/cars/Mobile - Grand Theft Auto 4 - Miscellaneous - Cars.png")
                        .SetColorKey(true, 0xa3a30d)),

        world(world),
        game_loop(game_loop),
        player_id(player_id),
        event_handler(game_loop, *this),

        // Inicializo los renderers
        world_renderer(renderer, map_texture, car_sprites, world, player_id),
        interface_renderer(renderer, font, font_small, font_big, map_texture, world, player_id),
        sound_manager() {

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
        
        std::cout << "[GAME] Todos los sonidos cargados correctamente" << std::endl;
        
        // iniciar música
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
    }
    
    update(dt);
    render();
    
    return true;
}


// para recalcular toda la UI
void Game::update_ui_layout() {
    int w = window.GetWidth();
    int h = window.GetHeight();

    // Actualizar layouts de los renderers
    world_renderer.update_layout(w, h);
    interface_renderer.update_layout(w, h);
}


void Game::process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) 
            return;

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
                    std::cout << "[GAME] Velocidad " << (speed_modified ? "activada" : "desactivada") << std::endl;
                }

                // BOTÓN DE SALUD
                if (interface_renderer.get_health_button_rect().Contains(x, y)) {
                    health_modified = !health_modified;
                    sound_manager.play_sound("button_click");
                    std::cout << "[GAME] Salud " << (health_modified ? "activada" : "desactivada") << std::endl;
                }
            }

            // BOTÓN GUARDAR
            if (interface_renderer.get_save_button_rect().Contains(x, y) && !saved) {
                saved = true; // no se puede deshacer como los otros
                sound_manager.play_sound("confirm");
             //   game_loop.send_modifications(speed_modified, health_modified);
                std::cout << "[GAME] ✓ Modificaciones enviadas!" << std::endl;
            }
            
        }
    }
}


game_state Game::get_current_state() const { return current_state; }

void Game::handle_modification_click(int x, int y) {
    if (interface_renderer.get_speed_button_rect().Contains(x, y))
        speed_modified = !speed_modified;

    if (interface_renderer.get_health_button_rect().Contains(x, y))
        health_modified = !health_modified;

    if (interface_renderer.get_save_button_rect().Contains(x, y)) {
        saved = !saved;
      //  game_loop.send_modifications(speed_modified, health_modified);
    }
}


void Game::update(float dt) {
    (void)dt;
    // actualizacion de world
    // obtiene el estado de todos los autos desde el World (que es actualizado por el Receiver)
    auto car_states = world.getCars();

    std::cout << "[GAME] Update: " << car_states.size() << " autos en world" << std::endl;

    race_timer_ms = world.getCountdown();

    // actualiza la posición y ángulo del auto del jugador local
    //  y mas adelante de los otros autos?
    if (car_states.count(player_id)) {
        const auto& my_car_state = car_states.at(player_id);

        std::cout << "[GAME] Mi auto: pos=(" << my_car_state.x << "," << my_car_state.y
                  << "), angle=" << my_car_state.angle << std::endl;

        // para actualizar la posición en pantalla del auto segun lo que dice el server
        //    player_car.set_state(my_car_state.x, my_car_state.y, my_car_state.angle);

        // Actualizar cámara
        world_renderer.update_camera(my_car_state.x, my_car_state.y);
    } else {
        std::cout << "[GAME] Mi auto (id=" << (int)player_id << ") NO está en el world!"
                  << std::endl;
    }

    if (active_cheat_notification != CheatType::NONE) {
        cheat_notification_timer -= dt;
        if (cheat_notification_timer <= 0)
            active_cheat_notification = CheatType::NONE;
    }
}


// metodos para que game actualice su state

void Game::start_race() {
    current_state = game_state::RACING;
    // desp recibo el tiempo del server supongo, por ahora lo pongo aca
    // race_timer = 600.0f;
}

void Game::set_race_timer(uint16_t time_ms) { this->race_timer_ms = time_ms; }

void Game::show_countdown(uint8_t number) {
    current_state = game_state::COUNTDOWN;
    countdown_number = number;
    countdown_timer = 0.0f;
}

void Game::show_stats(const RaceResults& results) {
    current_state = game_state::SHOWING_STATS;
    current_results = results;
    stats_timer_ms = results.countdown_ms;
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
    cheat_notification_timer = 3000.0f;
}


void Game::render() {
    renderer.Clear();

    // renderizar el mundo (mapa + autos) Con la camara y escalado
    world_renderer.render();

    // renderizar la ui sin la cámara y sin escalado
    switch (current_state) {
        case game_state::COUNTDOWN:
            interface_renderer.render_countdown(countdown_number);
            break;
        case game_state::RACING:
            interface_renderer.render_race_ui(race_timer_ms, current_race, total_races,
                                              window.GetWidth());
            break;
        case game_state::ELIMINATED:
            break;
        case game_state::SHOWING_STATS:
            interface_renderer.render_stats_popup(current_results, stats_timer_ms,
                                                  interface_renderer.get_speed_button_rect());
            break;
        case game_state::MODIFYING_CAR:
            interface_renderer.render_modification_popup(
                    speed_modified, health_modified, saved, mod_timer_ms,
                    interface_renderer.get_speed_button_rect());
            break;
        case game_state::GAME_END:
            break;
    }

    if (current_state == game_state::RACING)
        interface_renderer.render_minimap();
    
    if (active_cheat_notification != CheatType::NONE)
        interface_renderer.render_cheat_notif(active_cheat_notification);

    renderer.Present();
}
