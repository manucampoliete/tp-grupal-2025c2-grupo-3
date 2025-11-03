#include "ui_renderer.h"
#include "car.h"
#include <string>

UIRenderer::UIRenderer(Renderer& renderer, 
                       Font& font,
                       Texture& map_texture,
                       World& world,
                       uint8_t player_id)
    : renderer(renderer),
      font(font),
      map_texture(map_texture),
      world(world),
      player_id(player_id) {
}


void UIRenderer::update_layout(int window_width, int window_height) {
    // actualizo los popups
    int popup_w = static_cast<int>(window_width * 0.7f);
    int popup_h = static_cast<int>(window_height * 0.8f);
    int popup_x = (window_width - popup_w) / 2;
    int popup_y = (window_height - popup_h) / 2;
    
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
    int minimap_w = static_cast<int>(window_width * 0.2f); // 20% del ancho
    int minimap_h = static_cast<int>(minimap_w / map_aspect_ratio);
    int margin = static_cast<int>(window_width * 0.01f); // 1% de margen
    
    // esq derecha abajpo
    minimap_rect = Rect(window_width - minimap_w - margin, window_height - minimap_h - margin, minimap_w, minimap_h);
}


void UIRenderer::render_race_ui(uint32_t race_timer_ms, int current_race, int total_races, int window_width) {
    // reseteo el escalado para dibujar la UI que se dibuja 1:1 sobre la ventana
    renderer.SetScale(1.0f, 1.0f);
    
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 128);
    renderer.FillRect(Rect(0, 0, window_width, 40)); // 40px fijos, o podria ser h * 0.05

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
    int x_pos = window_width - race_texture.GetWidth() - 10;
    renderer.Copy(race_texture, NullOpt, Rect(x_pos, 5, race_texture.GetWidth(), race_texture.GetHeight()));
}


void UIRenderer::render_stats_popup(const RaceResults& current_results, uint32_t stats_timer_ms, const Rect& stats_popup_rect) {
    // reseteo el escalado para la UI
    renderer.SetScale(1.0f, 1.0f);
    
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 180);
    renderer.FillRect(Rect(0, 0, renderer.GetOutputWidth(), renderer.GetOutputHeight()));

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


void UIRenderer::render_modification_popup(bool speed_modified, bool accel_modified, uint32_t mod_timer_ms, const Rect& mod_popup_rect) {
    // reseteo el escalado para la UI
    renderer.SetScale(1.0f, 1.0f);
    
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 180);
    renderer.FillRect(Rect(0, 0, renderer.GetOutputWidth(), renderer.GetOutputHeight()));

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


void UIRenderer::render_minimap() {
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