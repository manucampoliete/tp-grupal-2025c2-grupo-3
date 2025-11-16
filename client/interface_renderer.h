#ifndef INTERFACE_RENDERER_H
#define INTERFACE_RENDERER_H

#include <cstdint>
#include <map>

#include <SDL2pp/Font.hh>
#include <SDL2pp/Rect.hh>
#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>

#include "../common/messages/game_data.h"

#include "world.h"

using namespace SDL2pp;  // NOLINT


class UIRenderer {
private:
    Renderer& renderer;
    Font& font;
    Font& font_small;
    Font& font_big;
    Texture& map_texture;
    World& world;
    uint8_t player_id;

    Texture& cheat_immortality_img;
    Texture& cheat_win_img;
    Texture& cheat_lose_img;

    Rect stats_popup_rect;
    Rect mod_popup_rect;
    Rect speed_button_rect;
    Rect health_button_rect;
    Rect save_button_rect;

    // minimapa: 20% del ancho de la ventana
    // esq derecha abajo
    Rect minimap_rect;

public:
    UIRenderer(Renderer& renderer, Font& font, Font& font_small, Font& font_big,
               Texture& map_texture, World& world, uint8_t player_id,
               Texture& cheat_immortality_img, Texture& cheat_win_img, Texture& cheat_lose_img);

    void render_countdown(uint8_t countdown_number);

    void render_race_ui(uint32_t race_timer_ms, int current_race, int total_races,
                        int window_width);

    void render_stats_popup(const RaceResults& current_results, uint32_t stats_timer_ms);

    void render_modification_popup(bool speed_modified, bool health_modified, bool saved,
                                   uint32_t mod_timer_ms, const CarProperties& props);

    void render_cheat_notif(CheatType active_cheat_notification);

    void render_eliminated_popup();

    void render_podium(const FinalResults& results);

    void render_minimap();

    // para recalcular toda la UI
    void update_layout(int window_width, int window_height);

    const Rect& get_speed_button_rect() const { return speed_button_rect; }
    const Rect& get_health_button_rect() const { return health_button_rect; }
    const Rect& get_save_button_rect() const { return save_button_rect; }
};

#endif  // INTERFACE_RENDERER_H
