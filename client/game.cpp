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
    camera(0, 0, 800, 600) { 

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    update_ui_layout(); 
}


void Game::run() {
    // en cada tick del juego hay tres acciones: procesar entradas, actualizar el estado y renderizar
    /*
    unsigned int prev_ticks = SDL_GetTicks();

    while (is_running) {
        unsigned int frame_ticks = SDL_GetTicks();
        unsigned int frame_delta = frame_ticks - prev_ticks;
        prev_ticks = frame_ticks;

        if (current_state == game_state::RACING) {
            if (!event_handler.handle_events())
                this->is_running = false;
        } else {
            process_input(); 
        }

        update(frame_delta);
        render();

        SDL_Delay(1);
    }
    */
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
    
    // actualizo el factor de escala
    scale_factor = (float)h / REFERENCE_HEIGHT;

    // actualizo la cámara para que coincida con el tamaño de la ventana
    // pero escalado inversamente (si la ventana es 2x, la cámara ve la mitad)
    // a chequear el zoom!!!
    camera.w = static_cast<int>(w / scale_factor);
    camera.h = static_cast<int>(h / scale_factor);

    // actualizo los popups
    int popup_w = static_cast<int>(w * 0.7f);
    int popup_h = static_cast<int>(h * 0.8f);
    int popup_x = (w - popup_w) / 2;
    int popup_y = (h - popup_h) / 2;
    
    stats_popup_rect = Rect(popup_x, popup_y, popup_w, popup_h);
    mod_popup_rect = Rect(popup_x, popup_y, popup_w, popup_h);

    // actualizo los botones (posiciones relativas al popup de mods)
    // chequear!!
    int btn_w = static_cast<int>(popup_w * 0.4f);
    int btn_h = static_cast<int>(popup_h * 0.1f);
    int margin_x = static_cast<int>(popup_w * 0.1f);
    int margin_y = static_cast<int>(popup_h * 0.1f);

    speed_button_rect = Rect(popup_x + margin_x, popup_y + margin_y * 2, btn_w, btn_h);
    accel_button_rect = Rect(popup_x + margin_x, popup_y + margin_y * 4, btn_w, btn_h);
    save_button_rect = Rect(popup_x + (popup_w - btn_w) / 2, popup_y + margin_y * 7, btn_w, btn_h);

    // minimapa
    // un 20% del ancho de la ventana
    float map_aspect_ratio = (float)map_texture.GetWidth() / (float)map_texture.GetHeight();
    int minimap_w = static_cast<int>(w * 0.2f); // 20% del ancho
    int minimap_h = static_cast<int>(minimap_w / map_aspect_ratio);
    int margin = static_cast<int>(w * 0.01f); // 1% de margen
    
    // esq derecha abajpo
    minimap_rect = Rect(w - minimap_w - margin, h - minimap_h - margin, minimap_w, minimap_h);
}


void Game::process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            is_running = false;
            return;
        }

        if (current_state == game_state::MODIFYING_CAR && event.type == SDL_MOUSEBUTTONDOWN) {
            int x = event.button.x;
            int y = event.button.y;

            if (speed_button_rect.Contains(x, y))
                speed_modified = !speed_modified;

            if (accel_button_rect.Contains(x, y))
                accel_modified = !accel_modified;

            if (save_button_rect.Contains(x, y)) {
                current_state = game_state::RACING;
                race_timer_ms = 60000;
                current_race++;
            }
        }
    }
}


game_state Game::get_current_state() const {
    return current_state;
}

void Game::handle_modification_click(int x, int y) {
    if (speed_button_rect.Contains(x, y))
        speed_modified = !speed_modified;

    if (accel_button_rect.Contains(x, y))
        accel_modified = !accel_modified;

    if (save_button_rect.Contains(x, y)) {
        client.send_modifications(speed_modified, accel_modified);
        current_state = game_state::RACING; // temporal, el server debería confirmar
        current_race++;
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
        
        camera.x = static_cast<int>(my_car_state.x - (camera.w / 2));
        camera.y = static_cast<int>(my_car_state.y - (camera.h / 2));

        // evito que la cámara se salga del mapa
        if (camera.x < 0) camera.x = 0;
        if (camera.y < 0) camera.y = 0;
        if (camera.x > map_texture.GetWidth() - camera.w) camera.x = map_texture.GetWidth() - camera.w;
        if (camera.y > map_texture.GetHeight() - camera.h) camera.y = map_texture.GetHeight() - camera.h;
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
    accel_modified = false;
}




// renderizado

void Game::render_map_camera() {
    // dibujp solo la porción de la cámara, escalada a toda la ventana    
    // aplico el escalado manualmente al renderer asi todo lo que dibujo es a escala
    renderer.SetScale(scale_factor, scale_factor);
    
    // dibujo la porción del mapa que ve la camara
    renderer.Copy(map_texture, camera, NullOpt);
}

void Game::render_all_cars() {
    auto cars = world.getCars();

    for (const auto& [id, car_state] : cars) {
        const Rect& src = CARS[car_state.type];
        
        // posición relativa a la cámara
        float screen_x = car_state.x - camera.x;
        float screen_y = car_state.y - camera.y;
        
        // el tamaño es el del sprite original, el SetScale() del renderer lo agranda
        Rect dest(screen_x, screen_y, src.GetW(), src.GetH());

        if (id == player_id) {
            // el set_state de player_car ya tiene las coordenadas del mundo
            // el render() debe hacer la conversión a pantalla
            player_car.render(camera, 1.0f); // 1.0f porque el renderer ya escala
        } else {
            // renderiza otros autos
            const Rect& src = CARS[id];
            float screen_x = car_state.x - camera.x;
            float screen_y = car_state.y - camera.y;
            Rect dest(screen_x, screen_y, src.GetW(), src.GetH());

            renderer.Copy(car_sprites, src, dest, car_state.angle);
        }
    }
}


void Game::render_race_ui() {
    // reseteo el escalado para dibujar la UI que se dibuja 1:1 sobre la ventana
    renderer.SetScale(1.0f, 1.0f);
    
    int w = window.GetWidth();
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 128);
    renderer.FillRect(Rect(0, 0, w, 40)); // 40px fijos, o podria ser h * 0.05

    int total_seconds = race_timer_ms / 1000;
    int minutes = total_seconds / 60;
    int seconds = total_seconds % 60;

    std::string time_text =
        (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
        (seconds < 10 ? "0" : "") + std::to_string(seconds);

    Surface time_surface = font.RenderText_Solid(time_text, {255, 255, 255, 255});
    Texture time_texture(renderer, time_surface);
    renderer.Copy(time_texture, NullOpt, Rect(10, 5, time_texture.GetWidth(), time_texture.GetHeight()));

    std::string race_text = "Carrera " + std::to_string(current_race) + "/" + std::to_string(total_races);
    Surface race_surface = font.RenderText_Solid(race_text, {255, 255, 0, 255});
    Texture race_texture(renderer, race_surface);
    int x_pos = w - race_texture.GetWidth() - 10;
    renderer.Copy(race_texture, NullOpt, Rect(x_pos, 5, race_texture.GetWidth(), race_texture.GetHeight()));
}

void Game::render_stats_popup() {
    // reseteo el escalado para la UI
    renderer.SetScale(1.0f, 1.0f);
    
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 180);
    renderer.FillRect(Rect(0, 0, window.GetWidth(), window.GetHeight()));

    // uso el rect calculado en update_ui_layout
    renderer.SetDrawColor(40, 40, 40, 255);
    renderer.FillRect(stats_popup_rect);

    Surface title_surface = font.RenderText_Solid("Resultados de la carrera", {255, 255, 0, 255});
    Texture title_texture(renderer, title_surface);
    int title_x = stats_popup_rect.x + (stats_popup_rect.w - title_texture.GetWidth()) / 2;
    renderer.Copy(title_texture, NullOpt, Rect(title_x, stats_popup_rect.y + 20, title_texture.GetWidth(), title_texture.GetHeight()));

    int row_y = stats_popup_rect.y + 80;
    for (const auto& player : current_results.players) {
        std::string name = player.player_name;
        std::string race_t = std::to_string(player.race_time_ms / 1000.0f) + "s";
        std::string total_t = std::to_string(player.total_time_ms / 1000.0f) + "s";
        
        std::string row = name + "     " + race_t + "     " + total_t;
        Surface row_surface = font.RenderText_Solid(row, {255, 255, 255, 255});
        Texture row_texture(renderer, row_surface);
        renderer.Copy(row_texture, NullOpt, Rect(stats_popup_rect.x + 50, row_y, row_texture.GetWidth(), row_texture.GetHeight()));
        row_y += 40;
    }
    
    // timer
    int seconds = stats_timer_ms / 1000;
    std::string timer_text = "Siguiente carrera en: " + std::to_string(seconds);
    Surface timer_surface = font.RenderText_Solid(timer_text, {255, 255, 255, 255});
    Texture timer_texture(renderer, timer_surface);
    int timer_x = stats_popup_rect.x + (stats_popup_rect.w - timer_texture.GetWidth()) / 2;
    renderer.Copy(timer_texture, NullOpt, Rect(timer_x, stats_popup_rect.y + stats_popup_rect.h - 50, timer_texture.GetWidth(), timer_texture.GetHeight()));
}

void Game::render_modification_popup() {
    // reseteo el escalado para la UI
    renderer.SetScale(1.0f, 1.0f);
    
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 180);
    renderer.FillRect(Rect(0, 0, window.GetWidth(), window.GetHeight()));

    // uso el Rect calculado en update_ui_layout
    renderer.SetDrawColor(40, 40, 40, 255);
    renderer.FillRect(mod_popup_rect);

    Surface title_surface = font.RenderText_Solid("Modificación de auto", {255, 255, 0, 255});
    Texture title_texture(renderer, title_surface);
    int title_x = mod_popup_rect.x + (mod_popup_rect.w - title_texture.GetWidth()) / 2;
    renderer.Copy(title_texture, NullOpt, Rect(title_x, mod_popup_rect.y + 20, title_texture.GetWidth(), title_texture.GetHeight()));

    // botones (rects responsivos)
    renderer.SetDrawColor(speed_modified ? 0 : 100, speed_modified ? 200 : 100, 100, 255);
    renderer.FillRect(speed_button_rect);
    Surface speed_surface = font.RenderText_Solid("Velocidad +5%", {255, 255, 255, 255});
    Texture speed_texture(renderer, speed_surface);
    renderer.Copy(speed_texture, NullOpt, Rect(speed_button_rect.x + 10, speed_button_rect.y + 10, speed_texture.GetWidth(), speed_texture.GetHeight()));

    renderer.SetDrawColor(accel_modified ? 0 : 100, accel_modified ? 200 : 100, 100, 255);
    renderer.FillRect(accel_button_rect);
    Surface accel_surface = font.RenderText_Solid("Aceleración +5%", {255, 255, 255, 255});
    Texture accel_texture(renderer, accel_surface);
    renderer.Copy(accel_texture, NullOpt, Rect(accel_button_rect.x + 10, accel_button_rect.y + 10, accel_texture.GetWidth(), accel_texture.GetHeight()));

    renderer.SetDrawColor(0, 150, 0, 255);
    renderer.FillRect(save_button_rect);
    Surface save_surface = font.RenderText_Solid("Guardar cambios", {255, 255, 255, 255});
    Texture save_texture(renderer, save_surface);
    renderer.Copy(save_texture, NullOpt, Rect(save_button_rect.x + 15, save_button_rect.y + 15, save_texture.GetWidth(), save_texture.GetHeight()));

    // timer
    int seconds = mod_timer_ms / 1000;
    std::string countdown = "Tiempo restante: " + std::to_string(seconds) + " s";
    Surface countdown_surface = font.RenderText_Solid(countdown, {255, 255, 255, 255});
    Texture countdown_texture(renderer, countdown_surface);
    int timer_x = mod_popup_rect.x + (mod_popup_rect.w - countdown_texture.GetWidth()) / 2;
    renderer.Copy(countdown_texture, NullOpt, Rect(timer_x, mod_popup_rect.y + mod_popup_rect.h - 50, countdown_texture.GetWidth(), countdown_texture.GetHeight()));
}



void Game::render_minimap() {
    // reseteo el escalado por las dudas
    renderer.SetScale(1.0f, 1.0f);

    // fondo semi-transparente
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 128); // negro, 50% opacidad
    renderer.FillRect(minimap_rect);

    // mapa completo, achicado (NullOpt es "copiar toda la textura")
    renderer.Copy(map_texture, NullOpt, minimap_rect);

    // autos como puntos para el minimap
    auto cars = world.getCars();
    float map_w = (float)map_texture.GetWidth();
    float map_h = (float)map_texture.GetHeight();

    for (const auto& [id, car_state] : cars) {
        float ratio_x = car_state.x / map_w;
        float ratio_y = car_state.y / map_h;

        int minimap_car_x = minimap_rect.x + static_cast<int>(ratio_x * minimap_rect.w);
        int minimap_car_y = minimap_rect.y + static_cast<int>(ratio_y * minimap_rect.h);

        // TEMPORAL!!!
        // ahora que uso el id como indice de auto hago este punto de color hardcodeado.
        // despues habria que chequear por auto segun id
        uint8_t car_type = id; 
        switch (car_type) {
            case 0: // verde (CARS[0])
                renderer.SetDrawColor(0, 255, 0, 255); 
                break;
            case 1: // rojo (CARS[1])
            case 2: // rojo (CARS[2])
            case 5: // rojo/negro (CARS[5])
                renderer.SetDrawColor(255, 0, 0, 255); 
                break;
            case 3: // azul (CARS[3])
            case 4: // azul (CARS[4])
                renderer.SetDrawColor(0, 150, 255, 255); 
                break;
            case 6: // violeta (CARS[6])
                renderer.SetDrawColor(128, 0, 128, 255); 
                break;
            default:
                renderer.SetDrawColor(255, 255, 255, 255);
                break;
        }
        
        // dibujoel punto del jugador un poco más grande
        if (id == player_id) {
            Rect car_dot_rect(minimap_car_x - 2, minimap_car_y - 2, 5, 5); // 5x5
            renderer.FillRect(car_dot_rect);
        } else {
            Rect car_dot_rect(minimap_car_x - 1, minimap_car_y - 1, 3, 3); // 3x3
            renderer.FillRect(car_dot_rect);
        }
    }

    // borde blanco
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
    renderer.SetDrawColor(255, 255, 255, 255);
    renderer.DrawRect(minimap_rect);
}


void Game::render() {
    renderer.Clear();

    // renderizar el mundo (mapa + autos) Con la camara y escalado
    render_map_camera();
    render_all_cars();

    // renderizar la ui sin la cámara y sin escalado
    switch (current_state) {
        case game_state::RACING:
            render_race_ui();
            break;
        case game_state::SHOWING_STATS:
            render_stats_popup();
            break;
        case game_state::MODIFYING_CAR:
            render_modification_popup();
            break;
    }

    if (current_state == game_state::RACING)
        render_minimap();

    renderer.Present();
}