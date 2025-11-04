#ifndef UI_RENDERER_H
#define UI_RENDERER_H

#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>
#include <SDL2pp/Font.hh>
#include <SDL2pp/Rect.hh>
#include <map>
#include <cstdint>

#include "../common/protocol/game_data.h"
#include "world.h"

using namespace SDL2pp;

class UIRenderer {
private:
    Renderer& renderer;
    Font& font;
    Texture& map_texture;
    World& world;
    uint8_t player_id;

    // actualizo los popups
    Rect stats_popup_rect;
    Rect mod_popup_rect;

    // actualizo los botones (posiciones relativas al popup de mods)
    // chequear!!
    Rect speed_button_rect;
    Rect accel_button_rect;
    Rect save_button_rect;

    // minimapa
    // un 20% del ancho de la ventana
    // esq derecha abajpo
    Rect minimap_rect;

public:
    UIRenderer(Renderer& renderer, Font& font, Texture& map_texture, World& world, uint8_t player_id);

    // renderizar la ui sin la cámara y sin escalado
    void render_countdown(uint8_t countdown_number);

    void render_race_ui(uint32_t race_timer_ms, int current_race, int total_races, int window_width);
    
    void render_stats_popup(const RaceResults& current_results, uint32_t stats_timer_ms);
    
    void render_modification_popup(bool speed_modified, bool accel_modified, uint32_t mod_timer_ms);
    
    // fondo semi-transparente
    // mapa completo, achicado (NullOpt es "copiar toda la textura")
    // autos como puntos para el minimap
    // borde blanco
    void render_minimap();

    // para recalcular toda la UI
    void update_layout(int window_width, int window_height);

    // Getters para los rectángulos (usado para detección de clicks)
    const Rect& get_speed_button_rect() const { return speed_button_rect; }
    const Rect& get_accel_button_rect() const { return accel_button_rect; }
    const Rect& get_save_button_rect() const { return save_button_rect; }
};

#endif // UI_RENDERER_H
