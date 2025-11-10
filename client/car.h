#ifndef CAR_H
#define CAR_H

#include <SDL.h>
#include <SDL2pp/Rect.hh>
#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>

using namespace SDL2pp;


// coordenadas (x, y, ancho, alto) para cada auto
const SDL2pp::Rect CARS[7] = {
        {134, 34, 20, 27},   // auto 1
        {170, 105, 20, 40},  // auto 2
        {170, 186, 20, 38},  // auto 3
        {170, 265, 20, 40},  // auto 4
        {170, 344, 20, 40},  // auto 5
        {170, 425, 20, 40},  // auto 6
        {205, 515, 20, 45},  // auto 7
};

// índices de direcciones
// despues con box2d no lo necesitariamos, esto lo definiria el angulo (creo)
enum Direction {
    UP,
    RIGHT,
    DOWN = 0,
    LEFT,
};

class Car {
private:
    Renderer& renderer;
    Texture& sprite_sheet;

    Rect src_rect;

    float x;
    float y;
    double rotation_angle = 0.0;  // angulo en grados

    // flags de movimiento
    bool move_up = false;
    bool move_down = false;
    bool move_left = false;
    bool move_right = false;

public:
    Car(Renderer& renderer, Texture& sprite_sheet, uint8_t car_type);
    void handle_event(const SDL_Event& event);
    void update(float dt);
    void render(const SDL2pp::Rect& camera, float scale_factor);
    void set_state(float x, float y, double angle);
};

#endif  // CAR_H
