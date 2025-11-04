#include "game.h"
#include "car.h"

#include <SDL2pp/SDL2pp.hh>
#include <SDL2pp/SDL.hh>


// muestro todo lo que pasa en el juego
// por ahi mas adelante cuando haya mas cosas convenga separar en mas clases


Game::Game(World& world, Client& client, uint8_t player_id) :
    sdl(SDL_INIT_VIDEO),
    ttf(),

    // ventana principal con título, pos automática y tamaño 800x600
    window("Need For Speed", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_RESIZABLE),
    
    // renderer acelerado por hardware
    renderer(window, -1, SDL_RENDERER_ACCELERATED),
    font("assets/fonts/VCR_OSD_MONO.ttf", 24), // font
    font_small("assets/fonts/VCR_OSD_MONO.ttf", 18),

    // cargo la textura del mapa desde un archivo
    // por ahora hardcodeo una cualquiera
    map_texture(renderer, "assets/cities/Game Boy _ GBC - Grand Theft Auto - Backgrounds - Vice City.png"),

    // cargo la textura con los sprites de autos
    car_sprites(renderer, SDL2pp::Surface("assets/cars/Mobile - Grand Theft Auto 4 - Miscellaneous - Cars.png").SetColorKey(true, 0xa3a30d)),

    world(world),
    client(client),
    player_id(player_id),

    // creo el auto del jugador
    player_car(renderer, car_sprites, player_id),
    event_handler(client, player_car, *this),

    // Inicializo los renderers
    world_renderer(renderer, map_texture, car_sprites, world, player_car, player_id),
    ui_renderer(renderer, font, font_small, map_texture, world, player_id) { 

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_SetWindowMinimumSize(window.Get(), 800, 600);
    update_ui_layout(); 
}


void Game::run() {
    const int FRAME_RATE = 30;
    const float FRAME_TIME_MS = 1000.0f / FRAME_RATE;

    float t1 = SDL_GetTicks();
    int it = 0;

    while (is_running) {
        // 1. procesar input y actualizar estado
        if (current_state == game_state::RACING) {
            if (!event_handler.handle_events()) {
                is_running = false;
            }
        } else {
            process_input();
        }
        update(FRAME_TIME_MS);
        render();

        // 2. sincronizar con el rate constante
        float t2 = SDL_GetTicks();
        float rest = FRAME_TIME_MS - (t2 - t1);

        if (rest < 0) {
            float behind = -rest;
            rest = FRAME_TIME_MS - fmod(behind, FRAME_TIME_MS);
            float lost = behind + rest;
            t1 += lost;
            it += static_cast<int>(lost / FRAME_TIME_MS);
        }

        SDL_Delay(static_cast<Uint32>(rest));
        t1 += FRAME_TIME_MS;
        it++;
    }
}

// para recalcular toda la UI
void Game::update_ui_layout() {
    int w = window.GetWidth();
    int h = window.GetHeight();
    
    // Actualizar layouts de los renderers
    world_renderer.update_layout(w, h);
    ui_renderer.update_layout(w, h);
}


void Game::process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            is_running = false;
            return;
        }

        if (current_state == game_state::COUNTDOWN)
            continue;

        if (current_state == game_state::MODIFYING_CAR && event.type == SDL_MOUSEBUTTONDOWN) {
            int x = event.button.x;
            int y = event.button.y;

            if (ui_renderer.get_speed_button_rect().Contains(x, y))
                speed_modified = !speed_modified;

            if (ui_renderer.get_health_button_rect().Contains(x, y))
                health_modified = !health_modified;

            if (ui_renderer.get_save_button_rect().Contains(x, y)) {
                saved = !saved;
            }
        }
    }
}


game_state Game::get_current_state() const {
    return current_state;
}

void Game::handle_modification_click(int x, int y) {
    if (ui_renderer.get_speed_button_rect().Contains(x, y))
        speed_modified = !speed_modified;

    if (ui_renderer.get_health_button_rect().Contains(x, y))
        health_modified = !health_modified;

    if (ui_renderer.get_save_button_rect().Contains(x, y)) {
        saved = !saved;
        client.send_modifications(speed_modified, health_modified);
    }
}


void Game::update(float dt) {
    (void)dt;
    // actualizacion de world
    // obtiene el estado de todos los autos desde el World (que es actualizado por el Receiver)
    auto car_states = world.getCars();

    // actualiza la posición y ángulo del auto del jugador local
    //  y mas adelante de los otros autos?
    if (car_states.count(player_id)) {
        const auto& my_car_state = car_states.at(player_id);
        // para actualizar la posición en pantalla del auto segun lo que dice el server
        player_car.set_state(my_car_state.x, my_car_state.y, my_car_state.angle);
        
        // Actualizar cámara
        world_renderer.update_camera(my_car_state.x, my_car_state.y);
    }


    // HARDCODEADO: manejar transiciones para testear lo visual
    if (current_state == game_state::COUNTDOWN) {
        countdown_timer += dt;
        
        if (countdown_timer >= 1000.0f) { // cada 1 seg
            countdown_timer = 0.0f;
            
            if (countdown_number > 0) {
                countdown_number--;
            } else {
                // despues del GO empezar carrera
                current_state = game_state::RACING;
                race_timer_ms = 20000; // resetear a 20 seg
            }
        }
    } else if (current_state == game_state::RACING) {
        if (race_timer_ms > dt) {
            race_timer_ms -= static_cast<uint32_t>(dt);
        } else {
            // cuando se acaba el tiempo, mostrar stats
            race_timer_ms = 0;
            
            // HARDCODEADO: resultados de prueba
            RaceResults fake_results;
            fake_results.countdown_ms = 10000; // 10 seg
            
            RaceResults::PlayerResult p1;
            p1.player_name = "Player1";
            p1.race_time_ms = 58000;
            p1.total_time_ms = 180000;
            fake_results.players.push_back(p1);
            
            RaceResults::PlayerResult p2;
            p2.player_name = "Player2";
            p2.race_time_ms = 60000;
            p2.total_time_ms = 185000;
            fake_results.players.push_back(p2);
            
            show_stats(fake_results);
        }
    } else if (current_state == game_state::SHOWING_STATS) {
        if (stats_timer_ms > dt) {
            stats_timer_ms -= static_cast<uint32_t>(dt);
        } else {
            // pasar a modificaciones
            stats_timer_ms = 0;
            
            // HARDCODEADO: propiedades de prueba
            CarProperties fake_props;
            fake_props.countdown_ms = 10000; // 10 seg
            show_modifications(fake_props);
        }
    } else if (current_state == game_state::MODIFYING_CAR) {
        if (mod_timer_ms > dt) {
            mod_timer_ms -= static_cast<uint32_t>(dt);
        } else {
            // volver a countdown para siguiente carrera
            mod_timer_ms = 0;
            countdown_number = 3;
            countdown_timer = 0.0f;
            current_state = game_state::COUNTDOWN;
            current_race++;
        }
    }
}


// metodos para que game actualice su state

void Game::start_race() {
    current_state = game_state::RACING;
    // desp recibo el tiempo del server supongo, por ahora lo pongo aca
   // race_timer = 600.0f; 
}

void Game::set_race_timer(uint16_t time_ms) {
    this->race_timer_ms = time_ms;
}

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


void Game::render() {
    renderer.Clear();

    // renderizar el mundo (mapa + autos) Con la camara y escalado
    world_renderer.render();

    // renderizar la ui sin la cámara y sin escalado
    switch (current_state) {
        case game_state::COUNTDOWN:
            ui_renderer.render_countdown(countdown_number);
            break;
        case game_state::RACING:
            ui_renderer.render_race_ui(race_timer_ms, current_race, total_races, window.GetWidth());
            break;
        case game_state::ELIMINATED:
            break;
        case game_state::SHOWING_STATS:
            ui_renderer.render_stats_popup(current_results, stats_timer_ms, ui_renderer.get_speed_button_rect()); 
            break;
        case game_state::MODIFYING_CAR:
            ui_renderer.render_modification_popup(speed_modified, health_modified, mod_timer_ms, ui_renderer.get_speed_button_rect()); 
            break;
        case game_state::GAME_END:
            break;
    }

    if (current_state == game_state::RACING)
        ui_renderer.render_minimap();
        
    renderer.Present();
}