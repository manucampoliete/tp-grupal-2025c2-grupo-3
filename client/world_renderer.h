#ifndef WORLD_RENDERER_H
#define WORLD_RENDERER_H

#include <cstdint>
#include <map>

#include <SDL.h>
#include <SDL2pp/Rect.hh>
#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>

#include "../common/messages/game_data.h"
#include "world.h"

using namespace SDL2pp;


class WorldRenderer {
private:
    Renderer& renderer;
    Texture& map_texture;
    Texture& car_sprites;
    World& world;
    uint8_t player_id;

    Rect camera;
    float scale_factor;
    const float REFERENCE_HEIGHT = 600.0f;

    std::vector<Explosion> explosions;
    std::vector<CollisionEffect> collision_effects;

    void render_map_camera();
    void render_all_cars();
    void render_explosions();
    void render_collision_effects();

public:
    WorldRenderer(Renderer& renderer, Texture& map_texture, Texture& car_sprites, World& world,
                  uint8_t player_id);

    // renderizar el mundo (mapa + autos) Con la camara y escalado
    void render();
    void update_layout(int window_width, int window_height);
    void update_camera(float player_x, float player_y);
    void update_effects(float dt);

    const Rect& get_camera() const { return camera; }
    float get_scale_factor() const { return scale_factor; }

    void add_explosion(float x, float y, int particle_count = 30);
    void add_collision_effect(float x, float y, float intensity);

};

#endif  // WORLD_RENDERER_H
