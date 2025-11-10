#ifndef WORLD_RENDERER_H
#define WORLD_RENDERER_H

#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>
#include <SDL2pp/Rect.hh>
#include <map>
#include <cstdint>

#include "world.h"
#include "car.h"

using namespace SDL2pp;

class WorldRenderer {
private:
    Renderer& renderer;
    Texture& map_texture;
    Texture& car_sprites;
    World& world;
 //   Car& player_car;
    uint8_t player_id;

    Rect camera;
    float scale_factor;
    const float REFERENCE_HEIGHT = 600.0f;

public:
    WorldRenderer(Renderer& renderer, 
                  Texture& map_texture, 
                  Texture& car_sprites,
                  World& world,
                  /*Car& player_car,*/
                  uint8_t player_id);

    // renderizar el mundo (mapa + autos) Con la camara y escalado
    void render();

    // actualiza la cámara para que coincida con el tamaño de la ventana
    // pero escalado inversamente (si la ventana es 2x, la cámara ve la mitad)
    // a chequear el zoom!!!
    void update_layout(int window_width, int window_height);

    // actualiza la posición y ángulo del auto del jugador local
    // evito que la cámara se salga del mapa
    void update_camera(float player_x, float player_y);

    const Rect& get_camera() const { return camera; }
    float get_scale_factor() const { return scale_factor; }

private:
    // dibujp solo la porción de la cámara, escalada a toda la ventana    
    // aplico el escalado manualmente al renderer asi todo lo que dibujo es a escala
    void render_map_camera();

    void render_all_cars();
};

#endif // WORLD_RENDERER_H