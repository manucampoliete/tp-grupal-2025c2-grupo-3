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
    event_handler(client, player_car) {

    // supuestamente mejora la calidad del mapa, no lo noto
    // "activa el suavizado bilineal en el escalado"
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    map_texture.SetBlendMode(SDL_BLENDMODE_BLEND);

    // Color Keying
    // creo que no puedo usarlo sin image

    // posiciones iniciales de botones en la fase de modificación
    // no estoy segura si esto va aca,
    // chequear para pantalla completa como hacer que se amolde
    int popup_w = 500;
    int popup_h = 400;
    int popup_x = (window.GetWidth() - popup_w) / 2;
    int popup_y = (window.GetHeight() - popup_h) / 2;

    speed_button_rect = Rect(popup_x + 50, popup_y + 100, 180, 50);
    accel_button_rect = Rect(popup_x + 50, popup_y + 180, 180, 50);
    save_button_rect = Rect(popup_x + 150, popup_y + 300, 200, 60);
}


// implementar el constant loop rate

void Game::run() {
    // en cada tick del juego hay tres acciones: procesar entradas, actualizar el estado y renderizar
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
                race_timer = 50.0f;
                current_race++;
            }
        }
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
        // para actualizar la posición en pantalla de tu propio auto según lo que dice el server
        player_car.set_state(my_car_state.x, my_car_state.y, my_car_state.angle);
    }

    // actualizo el race_timer desde world.getCountdown() asi el tiempo en pantalla queda sincronizado con el del server
    // hayq ue probar que funcione
    this->race_timer = world.getCountdown() / 1000.f;
}



// metodos para que game actualice su state

void Game::start_race() {
    current_state = game_state::RACING;
    // desp recibo el tiempo del server supongo, por ahora lo pongo aca
    race_timer = 600.0f; 
}

void Game::show_stats() {
    current_state = game_state::SHOWING_STATS;
}

void Game::show_modifications() {
    current_state = game_state::MODIFYING_CAR;
}





// renderizado

void Game::render_all_cars() {
    auto cars = world.getCars();

    for (const auto& [id, car_state] : cars) {
        const Rect& src = CARS[car_state.type];  // usa el tipo de auto real

        Rect dest(car_state.x, car_state.y, src.GetW(), src.GetH());

        if (id == player_id) {
            // el jugador local usa su auto, que ya maneja animaciones y rotación
            player_car.render();
        } else {
            // otros autos 
            const Rect& src = CARS[id];
            Rect dest(car_state.x, car_state.y, src.GetW(), src.GetH());
            
            // el angulo no funciona proque els erver no lo manda todavia
            renderer.Copy(car_sprites, src, dest, car_state.angle);
        }
    }
}


void Game::render_race_ui() {
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 128);
    renderer.FillRect(Rect(0, 0, window.GetWidth(), 40));

    int minutes = static_cast<int>(race_timer) / 60;
    int seconds = static_cast<int>(race_timer) % 60;

    std::string time_text =
        (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
        (seconds < 10 ? "0" : "") + std::to_string(seconds);

    Surface time_surface = font.RenderText_Solid(time_text, {255, 255, 255, 255});
    Texture time_texture(renderer, time_surface);
    renderer.Copy(time_texture, NullOpt, Rect(10, 5, time_texture.GetWidth(), time_texture.GetHeight()));

    // número de carrera actual (ej: "Carrera 2/6")
    std::string race_text = "Carrera " + std::to_string(current_race) + "/" + std::to_string(total_races);
    Surface race_surface = font.RenderText_Solid(race_text, {255, 255, 0, 255});
    Texture race_texture(renderer, race_surface);
    int x_pos = window.GetWidth() - race_texture.GetWidth() - 10;
    renderer.Copy(race_texture, NullOpt, Rect(x_pos, 5, race_texture.GetWidth(), race_texture.GetHeight()));
}

void Game::render_stats_popup() {
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 180);
    renderer.FillRect(Rect(0, 0, window.GetWidth(), window.GetHeight()));

    int popup_w = 500;
    int popup_h = 400;
    int popup_x = (window.GetWidth() - popup_w) / 2;
    int popup_y = (window.GetHeight() - popup_h) / 2;
    renderer.SetDrawColor(40, 40, 40, 255);
    renderer.FillRect(Rect(popup_x, popup_y, popup_w, popup_h));

    // título
    Surface title_surface = font.RenderText_Solid("Resultados de la carrera", {255, 255, 0, 255});
    Texture title_texture(renderer, title_surface);
    renderer.Copy(title_texture, NullOpt, Rect(popup_x + 80, popup_y + 20, title_texture.GetWidth(), title_texture.GetHeight()));

    // datos simulados
    std::vector<std::tuple<std::string, std::string, std::string>> players = {
        {"Camila", "00:48", "00:48"},
        {"Lucas", "00:52", "01:40"},
        {"Sofia", "00:54", "02:34"}
    };

    int row_y = popup_y + 100;
    for (auto& [name, race_t, total_t] : players) {
        std::string row = name + "     " + race_t + "     " + total_t;
        Surface row_surface = font.RenderText_Solid(row, {255, 255, 255, 255});
        Texture row_texture(renderer, row_surface);
        renderer.Copy(row_texture, NullOpt, Rect(popup_x + 50, row_y, row_texture.GetWidth(), row_texture.GetHeight()));
        row_y += 40;
    }
}

void Game::render_modification_popup() {
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 180);
    renderer.FillRect(Rect(0, 0, window.GetWidth(), window.GetHeight()));

    int popup_w = 500;
    int popup_h = 400;
    int popup_x = (window.GetWidth() - popup_w) / 2;
    int popup_y = (window.GetHeight() - popup_h) / 2;
    renderer.SetDrawColor(40, 40, 40, 255);
    renderer.FillRect(Rect(popup_x, popup_y, popup_w, popup_h));

    // título
    Surface title_surface = font.RenderText_Solid("Modificación de auto", {255, 255, 0, 255});
    Texture title_texture(renderer, title_surface);
    renderer.Copy(title_texture, NullOpt, Rect(popup_x + 100, popup_y + 20, title_texture.GetWidth(), title_texture.GetHeight()));

    // botones
    renderer.SetDrawColor(speed_modified ? 0 : 100, speed_modified ? 200 : 100, 100, 255);
    renderer.FillRect(speed_button_rect);
    Surface speed_surface = font.RenderText_Solid("Velocidad +5%", {255, 255, 255, 255});
    Texture speed_texture(renderer, speed_surface);
    renderer.Copy(speed_texture, NullOpt, Rect(speed_button_rect.GetX() + 10, speed_button_rect.GetY() + 10, speed_texture.GetWidth(), speed_texture.GetHeight()));

    renderer.SetDrawColor(accel_modified ? 0 : 100, accel_modified ? 200 : 100, 100, 255);
    renderer.FillRect(accel_button_rect);
    Surface accel_surface = font.RenderText_Solid("Aceleración +5%", {255, 255, 255, 255});
    Texture accel_texture(renderer, accel_surface);
    renderer.Copy(accel_texture, NullOpt, Rect(accel_button_rect.GetX() + 10, accel_button_rect.GetY() + 10, accel_texture.GetWidth(), accel_texture.GetHeight()));

    renderer.SetDrawColor(0, 150, 0, 255);
    renderer.FillRect(save_button_rect);
    Surface save_surface = font.RenderText_Solid("Guardar cambios", {255, 255, 255, 255});
    Texture save_texture(renderer, save_surface);
    renderer.Copy(save_texture, NullOpt, Rect(save_button_rect.GetX() + 15, save_button_rect.GetY() + 15, save_texture.GetWidth(), save_texture.GetHeight()));

    // contador
    std::string countdown = "Tiempo restante: " + std::to_string((int)modification_timer) + " s";
    Surface countdown_surface = font.RenderText_Solid(countdown, {255, 255, 255, 255});
    Texture countdown_texture(renderer, countdown_surface);
    renderer.Copy(countdown_texture, NullOpt, Rect(popup_x + 120, popup_y + 360, countdown_texture.GetWidth(), countdown_texture.GetHeight()));
}

void Game::render() {
    renderer.Clear();
    renderer.Copy(map_texture);

    render_all_cars();

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

    renderer.Present();
}