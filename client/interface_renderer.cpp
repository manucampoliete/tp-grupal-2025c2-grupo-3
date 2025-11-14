#include "interface_renderer.h"

#include <string>

UIRenderer::UIRenderer(Renderer& renderer, Font& font, Font& font_small, Font& font_big, Texture& map_texture,
                       World& world, uint8_t player_id):
        renderer(renderer),
        font(font),
        font_small(font_small),
        font_big(font_big),
        map_texture(map_texture),
        world(world),
        player_id(player_id) {}


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
    health_button_rect = Rect(popup_x + margin_x, popup_y + margin_y * 4, btn_w, btn_h);
    save_button_rect = Rect(popup_x + (popup_w - btn_w) / 2, popup_y + margin_y * 7, btn_w, btn_h);

    // minimapa
    // un 20% del ancho de la ventana
    float map_aspect_ratio = (float)map_texture.GetWidth() / (float)map_texture.GetHeight();
    int minimap_w = static_cast<int>(window_width * 0.2f);  // 20% del ancho
    int minimap_h = static_cast<int>(minimap_w / map_aspect_ratio);
    int margin = static_cast<int>(window_width * 0.01f);  // 1% de margen

    // esq derecha abajpo
    minimap_rect = Rect(window_width - minimap_w - margin, window_height - minimap_h - margin,
                        minimap_w, minimap_h);
}


void UIRenderer::render_countdown(uint8_t countdown_number) {
    // resetear escalado
    renderer.SetScale(1.0f, 1.0f);

    // overlay oscuro semi-transparente
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 150);
    renderer.FillRect(Rect(0, 0, renderer.GetOutputWidth(), renderer.GetOutputHeight()));

    // texto del countdown
    std::string text;
    SDL_Color color;

    if (countdown_number == 3) {
        text = "3";
        color = {255, 0, 0, 255};  // rojo
    } else if (countdown_number == 2) {
        text = "2";
        color = {255, 165, 0, 255};  // naranja
    } else if (countdown_number == 1) {
        text = "1";
        color = {255, 255, 0, 255};  // amarillo
    } else {                         // 0 = GO!
        text = "GO!";
        color = {0, 255, 0, 255};  // verde
    }

    // crear fuente gigante para el countdown
    Font big_font("client/assets/fonts/VCR_OSD_MONO.ttf", 140);
    Surface s = big_font.RenderText_Solid(text, color);
    Texture t(renderer, s);

    // centrar en pantalla
    int x = (renderer.GetOutputWidth() - t.GetWidth()) / 2;
    int y = (renderer.GetOutputHeight() - t.GetHeight()) / 2;

    renderer.Copy(t, NullOpt, Rect(x, y, t.GetWidth(), t.GetHeight()));
}


void UIRenderer::render_race_ui(uint32_t race_timer_ms, int current_race, int total_races,
                                int window_width) {
    // reseteo el escalado para dibujar la UI que se dibuja 1:1 sobre la ventana
    renderer.SetScale(1.0f, 1.0f);

    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 128);
    renderer.FillRect(Rect(0, 0, window_width, 40));  // 40px fijos, o podria ser h * 0.05

    int total_seconds = race_timer_ms;
    int minutes = total_seconds / 60;
    int seconds = total_seconds % 60;

    std::string time_text = (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
                            (seconds < 10 ? "0" : "") + std::to_string(seconds);

    Surface time_surface = font.RenderText_Solid(time_text, {255, 255, 255, 255});
    Texture time_texture(renderer, time_surface);
    renderer.Copy(time_texture, NullOpt,
                  Rect(10, 5, time_texture.GetWidth(), time_texture.GetHeight()));

    std::string race_text =
            "Race " + std::to_string(current_race) + "/" + std::to_string(total_races);
    Surface race_surface = font.RenderText_Solid(race_text, {255, 255, 0, 255});
    Texture race_texture(renderer, race_surface);
    int x_pos = window_width - race_texture.GetWidth() - 10;
    renderer.Copy(race_texture, NullOpt,
                  Rect(x_pos, 5, race_texture.GetWidth(), race_texture.GetHeight()));
}


void UIRenderer::render_stats_popup(const RaceResults& current_results, uint32_t stats_timer_ms,
                                    const Rect& dummy_rect) {
    (void)dummy_rect;  // no se usa
    // recalculo para hacer que el popup sea responsive

    // reseteo el escalado para la UI
    renderer.SetScale(1.0f, 1.0f);

    // recalcular popup centrado según tamaño actual de ventana
    int w = renderer.GetOutputWidth();
    int h = renderer.GetOutputHeight();
    int popup_w = static_cast<int>(w * 0.7f);
    int popup_h = static_cast<int>(h * 0.8f);
    int popup_x = (w - popup_w) / 2;
    int popup_y = (h - popup_h) / 2;
    Rect stats_popup_rect(popup_x, popup_y, popup_w, popup_h);

    // elegir fuente según tamaño de ventana
    Font& active_font = (w < 1000) ? font_small : font;

    // overlay oscuro de fondo
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 180);
    renderer.FillRect(Rect(0, 0, w, h));

    renderer.SetDrawColor(40, 40, 40, 255);
    renderer.FillRect(stats_popup_rect);

    // borde del popup
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
    renderer.SetDrawColor(100, 100, 150, 255);
    renderer.DrawRect(stats_popup_rect);

    Surface title_surface = active_font.RenderText_Solid("RACE RESULTS", {255, 255, 0, 255});
    Texture title_texture(renderer, title_surface);
    int title_x = stats_popup_rect.x + (stats_popup_rect.w - title_texture.GetWidth()) / 2;
    renderer.Copy(title_texture, NullOpt,
                  Rect(title_x, stats_popup_rect.y + 30, title_texture.GetWidth(),
                       title_texture.GetHeight()));

    // ENCABEZADOS DE LA TABLA (centrados en columnas)
    int table_start_y = stats_popup_rect.y + 100;
    int row_height = (w < 1000) ? 40 : 50;  // filas más compactas en pantallas chicas

    // definir anchos de columnas (proporcionales al popup)
    int col_pos_w = static_cast<int>(stats_popup_rect.w * 0.15f);
    int col_name_w = static_cast<int>(stats_popup_rect.w * 0.35f);
    int col_race_w = static_cast<int>(stats_popup_rect.w * 0.25f);
    int col_total_w = static_cast<int>(stats_popup_rect.w * 0.25f);

    int col_pos_x = stats_popup_rect.x + 20;
    int col_name_x = col_pos_x + col_pos_w;
    int col_race_x = col_name_x + col_name_w;
    int col_total_x = col_race_x + col_race_w;

    // renderizar encabezados
    Surface h1_s = active_font.RenderText_Solid("POS", {200, 200, 200, 255});
    Texture h1_t(renderer, h1_s);
    renderer.Copy(h1_t, NullOpt, Rect(col_pos_x, table_start_y, h1_t.GetWidth(), h1_t.GetHeight()));

    Surface h2_s = active_font.RenderText_Solid("PLAYER", {200, 200, 200, 255});
    Texture h2_t(renderer, h2_s);
    renderer.Copy(h2_t, NullOpt,
                  Rect(col_name_x, table_start_y, h2_t.GetWidth(), h2_t.GetHeight()));

    Surface h3_s = active_font.RenderText_Solid("RACE TIME", {200, 200, 200, 255});
    Texture h3_t(renderer, h3_s);
    renderer.Copy(h3_t, NullOpt,
                  Rect(col_race_x, table_start_y, h3_t.GetWidth(), h3_t.GetHeight()));

    Surface h4_s = active_font.RenderText_Solid("TOTAL TIME", {200, 200, 200, 255});
    Texture h4_t(renderer, h4_s);
    renderer.Copy(h4_t, NullOpt,
                  Rect(col_total_x, table_start_y, h4_t.GetWidth(), h4_t.GetHeight()));

    // línea separadora bajo encabezados
    renderer.SetDrawColor(100, 100, 150, 255);
    int line_y = table_start_y + 35;
    renderer.DrawLine(col_pos_x, line_y, col_total_x + col_total_w - 20, line_y);

    // FILAS DE JUGADORES
    int row_y = line_y + 15;
    int position = 1;

    for (const auto& player: current_results.players) {
        // posición
        std::string pos_text = std::to_string(position) + "°";
        SDL_Color row_color = (position == 1) ? SDL_Color{255, 215, 0, 255} : // dorado para 1°
                              (position == 2) ? SDL_Color{192, 192, 192, 255} : // plateado para 2°
                              (position == 3) ? SDL_Color{205, 127, 50, 255} : // bronce para 3°
                              SDL_Color{255, 255, 255, 255}; // blanco para el resto

        Surface pos_s = active_font.RenderText_Solid(pos_text, row_color);
        Texture pos_t(renderer, pos_s);
        renderer.Copy(pos_t, NullOpt, Rect(col_pos_x, row_y, pos_t.GetWidth(), pos_t.GetHeight()));

        // nombre
        Surface name_s = active_font.RenderText_Solid(player.player_name, row_color);
        Texture name_t(renderer, name_s);
        renderer.Copy(name_t, NullOpt,
                      Rect(col_name_x, row_y, name_t.GetWidth(), name_t.GetHeight()));

        // tiempo de carrera
        int race_minutes = player.race_time_ms / 60000;
        int race_seconds = (player.race_time_ms % 60000) / 1000;
        int race_millis = player.race_time_ms % 1000;
        char race_time_buf[32];
        snprintf(race_time_buf, sizeof(race_time_buf), "%02d:%02d.%03d", race_minutes, race_seconds,
                 race_millis);

        Surface race_s = active_font.RenderText_Solid(race_time_buf, row_color);
        Texture race_t(renderer, race_s);
        renderer.Copy(race_t, NullOpt,
                      Rect(col_race_x, row_y, race_t.GetWidth(), race_t.GetHeight()));

        // tiempo total
        int total_minutes = player.total_time_ms / 60000;
        int total_seconds = (player.total_time_ms % 60000) / 1000;
        int total_millis = player.total_time_ms % 1000;
        char total_time_buf[32];
        snprintf(total_time_buf, sizeof(total_time_buf), "%02d:%02d.%03d", total_minutes,
                 total_seconds, total_millis);

        Surface total_s = active_font.RenderText_Solid(total_time_buf, row_color);
        Texture total_t(renderer, total_s);
        renderer.Copy(total_t, NullOpt,
                      Rect(col_total_x, row_y, total_t.GetWidth(), total_t.GetHeight()));

        row_y += row_height;
        position++;
    }

    // COUNTDOWN (centrado abajo)
    int seconds = stats_timer_ms / 1000;
    std::string timer_text = "Next stage in: " + std::to_string(seconds) + "s";
    Surface timer_surface =
            active_font.RenderText_Solid(timer_text, {150, 255, 150, 255});  // verde claro
    Texture timer_texture(renderer, timer_surface);
    int timer_x = stats_popup_rect.x + (stats_popup_rect.w - timer_texture.GetWidth()) / 2;
    int timer_y = stats_popup_rect.y + stats_popup_rect.h - 60;
    renderer.Copy(timer_texture, NullOpt,
                  Rect(timer_x, timer_y, timer_texture.GetWidth(), timer_texture.GetHeight()));
}


void UIRenderer::render_modification_popup(bool speed_modified, bool health_modified, bool saved,
                                           uint32_t mod_timer_ms, const Rect& dummy_rect) {
    (void)dummy_rect;  // no se usa
    // recalculo para hacer que el popup sea responsive

    // reseteo el escalado para la UI
    renderer.SetScale(1.0f, 1.0f);

    // recalcular popup centrado y botones según tamaño actual de ventana
    int w = renderer.GetOutputWidth();
    int h = renderer.GetOutputHeight();
    int popup_w = static_cast<int>(w * 0.7f);
    int popup_h = static_cast<int>(h * 0.8f);
    int popup_x = (w - popup_w) / 2;
    int popup_y = (h - popup_h) / 2;
    Rect mod_popup_rect(popup_x, popup_y, popup_w, popup_h);

    // elegir fuente según tamaño de ventana
    Font& active_font = (w < 1000) ? font_small : font;

    // recalcular botones (posiciones relativas al popup recalculado)
    int btn_w = static_cast<int>(popup_w * 0.6f);
    int btn_h = static_cast<int>(popup_h * 0.15f);
    int margin_x = (popup_w - btn_w) / 2;  // centrar horizontalmente
    int margin_y = static_cast<int>(popup_h * 0.15f);

    Rect speed_btn(popup_x + margin_x, popup_y + margin_y * 1.5, btn_w, btn_h);
    Rect health_btn(popup_x + margin_x, popup_y + margin_y * 2.6, btn_w, btn_h);
    Rect save_btn(popup_x + margin_x, popup_y + popup_h - margin_y - btn_h - 50, btn_w, btn_h);

    // overlay oscuro de fondo
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 180);
    renderer.FillRect(Rect(0, 0, w, h));

    renderer.SetDrawColor(40, 40, 40, 255);
    renderer.FillRect(mod_popup_rect);

    // borde del popup
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
    renderer.SetDrawColor(100, 100, 150, 255);
    renderer.DrawRect(mod_popup_rect);

    Surface title_surface = active_font.RenderText_Solid("CAR MODIFICATIONS", {255, 255, 0, 255});
    Texture title_texture(renderer, title_surface);
    int title_x = mod_popup_rect.x + (mod_popup_rect.w - title_texture.GetWidth()) / 2;
    renderer.Copy(title_texture, NullOpt,
                  Rect(title_x, mod_popup_rect.y + 30, title_texture.GetWidth(),
                       title_texture.GetHeight()));

    // SUBTITULO
    Surface sub_surface = active_font.RenderText_Solid("Choose the updates you want for your car",
                                                       {180, 180, 180, 255});
    Texture sub_texture(renderer, sub_surface);
    int sub_x = mod_popup_rect.x + (mod_popup_rect.w - sub_texture.GetWidth()) / 2;
    renderer.Copy(
            sub_texture, NullOpt,
            Rect(sub_x, mod_popup_rect.y + 70, sub_texture.GetWidth(), sub_texture.GetHeight()));


    // BOTON DE VELOCIDAD
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(speed_modified ? 50 : 80, speed_modified ? 200 : 80,
                          speed_modified ? 50 : 100, 255);
    renderer.FillRect(speed_btn);

    // borde del botón
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
    renderer.SetDrawColor(speed_modified ? 100 : 60, speed_modified ? 255 : 100,
                          speed_modified ? 100 : 120, 255);
    renderer.DrawRect(speed_btn);

    // texto del botón (centrado verticalmente)
    Surface speed_surface = active_font.RenderText_Solid("Velocity +5%", {255, 255, 255, 255});
    Texture speed_texture(renderer, speed_surface);
    int speed_text_x = speed_btn.x + (speed_btn.w - speed_texture.GetWidth()) / 2;  // centrado
    int speed_text_y = speed_btn.y + 15;
    renderer.Copy(
            speed_texture, NullOpt,
            Rect(speed_text_x, speed_text_y, speed_texture.GetWidth(), speed_texture.GetHeight()));

    // penalización (derecha del botón)
    Surface speed_pen = active_font.RenderText_Solid("Cost: +10s", {255, 200, 100, 255});
    Texture speed_pen_t(renderer, speed_pen);
    int speed_pen_x = speed_btn.x + (speed_btn.w - speed_pen_t.GetWidth()) / 2;  // centrado
    int speed_pen_y = speed_text_y + speed_texture.GetHeight() + 10;
    renderer.Copy(speed_pen_t, NullOpt,
                  Rect(speed_pen_x, speed_pen_y, speed_pen_t.GetWidth(), speed_pen_t.GetHeight()));


    // BOTON DE HEALTH
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(health_modified ? 50 : 80, health_modified ? 200 : 80,
                          health_modified ? 50 : 100, 255);
    renderer.FillRect(health_btn);

    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
    renderer.SetDrawColor(health_modified ? 100 : 60, health_modified ? 255 : 100,
                          health_modified ? 100 : 120, 255);
    renderer.DrawRect(health_btn);

    Surface health_surface = active_font.RenderText_Solid("Health +5%", {255, 255, 255, 255});
    Texture health_texture(renderer, health_surface);
    int health_text_x = health_btn.x + (health_btn.w - health_texture.GetWidth()) / 2;  // centrado
    int health_text_y = health_btn.y + 15;
    renderer.Copy(health_texture, NullOpt,
                  Rect(health_text_x, health_text_y, health_texture.GetWidth(),
                       health_texture.GetHeight()));

    Surface health_pen = active_font.RenderText_Solid("Cost: +8s", {255, 200, 100, 255});
    Texture health_pen_t(renderer, health_pen);
    int health_pen_x = health_btn.x + (health_btn.w - health_pen_t.GetWidth()) / 2;  // centrado
    int health_pen_y = health_text_y + health_texture.GetHeight() + 10;
    renderer.Copy(
            health_pen_t, NullOpt,
            Rect(health_pen_x, health_pen_y, health_pen_t.GetWidth(), health_pen_t.GetHeight()));


    // BOTON GUARDAR
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    
    if (saved)
        // verde oscuro cuando ya se guardó (no se puede cambiar)
        renderer.SetDrawColor(30, 120, 30, 255);
    else
        // azul cuando no se guardó
        renderer.SetDrawColor(50, 150, 255, 255);
    renderer.FillRect(save_btn);
    
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
    
    if (saved)
        renderer.SetDrawColor(60, 200, 60, 255); // borde verde
    else
        renderer.SetDrawColor(100, 200, 255, 255); // borde azul
    renderer.DrawRect(save_btn);

    std::string save_text = saved ? "SAVED!" : "SAVE AND CONTINUE";
    Surface save_surface = active_font.RenderText_Solid(save_text, {255, 255, 255, 255});
    Texture save_texture(renderer, save_surface);
    int save_text_x = save_btn.x + (save_btn.w - save_texture.GetWidth()) / 2;
    int save_text_y = save_btn.y + (save_btn.h - save_texture.GetHeight()) / 2;
    renderer.Copy(
            save_texture, NullOpt,
            Rect(save_text_x, save_text_y, save_texture.GetWidth(), save_texture.GetHeight()));


    // COUNTDOWN (centrado abajo)
    int seconds = mod_timer_ms / 1000;
    std::string countdown = "Next race in: " + std::to_string(seconds) + "s";
    Surface countdown_surface =
            active_font.RenderText_Solid(countdown, {255, 150, 150, 255});  // rojo claro
    Texture countdown_texture(renderer, countdown_surface);
    int timer_x = mod_popup_rect.x + (mod_popup_rect.w - countdown_texture.GetWidth()) / 2;
    int timer_y = mod_popup_rect.y + mod_popup_rect.h - 40;
    renderer.Copy(
            countdown_texture, NullOpt,
            Rect(timer_x, timer_y, countdown_texture.GetWidth(), countdown_texture.GetHeight()));

    speed_button_rect = speed_btn;
    health_button_rect = health_btn;
    save_button_rect = save_btn;
}


void UIRenderer::render_minimap() {
    // reseteo el escalado por las dudas
    renderer.SetScale(1.0f, 1.0f);

    // fondo semi-transparente
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 128);  // negro, 50% opacidad
    renderer.FillRect(minimap_rect);

    // mapa completo, achicado (NullOpt es "copiar toda la textura")
    renderer.Copy(map_texture, NullOpt, minimap_rect);

    // autos como puntos para el minimap
    auto cars = world.getCars();
    float map_w = (float)map_texture.GetWidth();
    float map_h = (float)map_texture.GetHeight();

    for (const auto& [id, car_state]: cars) {
        float ratio_x = car_state.x / map_w;
        float ratio_y = car_state.y / map_h;

        int minimap_car_x = minimap_rect.x + static_cast<int>(ratio_x * minimap_rect.w);
        int minimap_car_y = minimap_rect.y + static_cast<int>(ratio_y * minimap_rect.h);

        // TEMPORAL!!!
        // ahora que uso el id como indice de auto hago este punto de color hardcodeado.
        // despues habria que chequear por auto segun id
        uint8_t car_type = car_state.type; 
        switch (car_type) {
            case 0:  // verde (CARS[0])
                renderer.SetDrawColor(0, 255, 0, 255);
                break;
            case 1:  // rojo (CARS[1])
            case 2:  // rojo (CARS[2])
            case 5:  // rojo/negro (CARS[5])
                renderer.SetDrawColor(255, 0, 0, 255);
                break;
            case 3:  // azul (CARS[3])
            case 4:  // azul (CARS[4])
                renderer.SetDrawColor(0, 150, 255, 255);
                break;
            case 6:  // violeta (CARS[6])
                renderer.SetDrawColor(128, 0, 128, 255);
                break;
            default:
                renderer.SetDrawColor(255, 255, 255, 255);
                break;
        }

        // dibujoel punto del jugador un poco más grande
        if (id == player_id) {
            Rect car_dot_rect(minimap_car_x - 2, minimap_car_y - 2, 5, 5);  // 5x5
            renderer.FillRect(car_dot_rect);
        } else {
            Rect car_dot_rect(minimap_car_x - 1, minimap_car_y - 1, 3, 3);  // 3x3
            renderer.FillRect(car_dot_rect);
        }
    }

    // borde blanco
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
    renderer.SetDrawColor(255, 255, 255, 255);
    renderer.DrawRect(minimap_rect);
}


void UIRenderer::render_cheat_notif(CheatType active_cheat_notification) {
    renderer.SetScale(1.0f, 1.0f);

    int w = renderer.GetOutputWidth();
    int h = renderer.GetOutputHeight();
    
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 180);
    renderer.FillRect(Rect(0, 0, w, h));
    
    int popup_w = static_cast<int>(w * 0.5f);
    int popup_h = static_cast<int>(h * 0.5f);
    int popup_x = (w - popup_w) / 2;
    int popup_y = (h - popup_h) / 2;
    
    renderer.SetDrawColor(40, 40, 40, 255);
    renderer.FillRect(Rect(popup_x, popup_y, popup_w, popup_h));
    
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
    renderer.SetDrawColor(100, 100, 150, 255);
    renderer.DrawRect(Rect(popup_x, popup_y, popup_w, popup_h));
    
    std::string title;
    std::string ascii_art;
    SDL_Color title_color;
    
    switch (active_cheat_notification) {
        case CheatType::INMORTALITY:
            title = "INMORTALITY";
            ascii_art = "( ! )";
            title_color = {255, 215, 0, 255};
            break;
        case CheatType::INSTA_WIN:
            title = "INSTA WIN!";
            ascii_art = "[WIN]";
            title_color = {0, 255, 0, 255};
            break;
        case CheatType::INSTA_LOSE:
            title = "INSTA LOSE";
            ascii_art = "[LOSE]";
            title_color = {255, 0, 0, 255};
            break;
        default:
            break;
    }
    
    Surface art_surface = font_big.RenderText_Solid(ascii_art, {255, 255, 255, 255});
    Texture art_texture(renderer, art_surface);
    
    int art_x = popup_x + (popup_w - art_texture.GetWidth()) / 2;
    int art_y = popup_y + 60;
    renderer.Copy(art_texture, NullOpt,
                    Rect(art_x, art_y, art_texture.GetWidth(), art_texture.GetHeight()));
    
    Surface title_surface = font.RenderText_Solid(title, title_color);
    Texture title_texture(renderer, title_surface);
    
    int title_x = popup_x + (popup_w - title_texture.GetWidth()) / 2;
    int title_y = art_y + art_texture.GetHeight() + 30; // 30px debajo del arte
    renderer.Copy(title_texture, NullOpt,
                    Rect(title_x, title_y, title_texture.GetWidth(), title_texture.GetHeight()));

    
    // 3. Renderizar el SUBTÍTULO usando la fuente 'font' estándar (que ya tenías)
    Surface sub_surface = font.RenderText_Solid("ACTIVATED", {200, 200, 200, 255});
    Texture sub_texture(renderer, sub_surface);
    
    int sub_x = popup_x + (popup_w - sub_texture.GetWidth()) / 2;
    int sub_y = title_y + title_texture.GetHeight() + 20; // 20px debajo del título
    renderer.Copy(sub_texture, NullOpt,
                    Rect(sub_x, sub_y, sub_texture.GetWidth(), sub_texture.GetHeight()));
}