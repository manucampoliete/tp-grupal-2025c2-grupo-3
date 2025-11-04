#include "car.h"


// a revisar, aca sigue habiendo muchas cosas hardcodeadas



// velocidad del auto en píxeles por milisegundo
// por ahora harcodeado como constante
const float CAR_SPEED = 0.2f;

Car::Car(Renderer& renderer, Texture& sprite_sheet, uint8_t car_type) :
    renderer(renderer),
    sprite_sheet(sprite_sheet),
    // elijo un auto hardcodeado para probar (por ejemplo el 3)
    src_rect(CARS[car_type]),
    x(0),
    y(0),
    rotation_angle(0.0)
{
    int window_w = renderer.GetOutputWidth();
    int window_h = renderer.GetOutputHeight();
    x = window_w / 2;
    y = window_h / 2;
}

// procesa eventos de teclado para activar/desactivar movimiento

void Car::handle_event(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_UP:    move_up = true; break;
            case SDLK_DOWN:  move_down = true; break;
            case SDLK_LEFT:  move_left = true; break;
            case SDLK_RIGHT: move_right = true; break;
        }
    } else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
            case SDLK_UP:    move_up = false; break;
            case SDLK_DOWN:  move_down = false; break;
            case SDLK_LEFT:  move_left = false; break;
            case SDLK_RIGHT: move_right = false; break;
        }
    }
}

// actualiza posición y ángulo según el movimiento
// ya no seria necesaria porque el update lo hace game
// no se si seria util para los npc, seria como que estan hardcodeados?
/*
void Car::update(float dt) {
    float dx = 0.0f;
    float dy = 0.0f;

    if (move_up) {
        dy -= CAR_SPEED * dt;
        rotation_angle = 0.0; // mirando hacia arriba
    }
    if (move_down) {
        dy += CAR_SPEED * dt;
        rotation_angle = 180.0; // hacia abajo
    }
    if (move_left) {
        dx -= CAR_SPEED * dt;
        rotation_angle = 270.0; // hacia la izquierda
    }
    if (move_right) {
        dx += CAR_SPEED * dt;
        rotation_angle = 90.0; // hacia la derecha
    }

    dest_rect.x += dx;
    dest_rect.y += dy;
}
*/

void Car::render(const SDL2pp::Rect& camera, float scale_factor) {
    // calculo el tamaño escalado del auto
    int scaled_w = static_cast<int>(src_rect.GetW() * scale_factor);
    int scaled_h = static_cast<int>(src_rect.GetH() * scale_factor);

    // calculo la posicion en pantalla (relativa a la camara) y la escalo
    int screen_x = static_cast<int>((x - camera.x) * scale_factor);
    int screen_y = static_cast<int>((y - camera.y) * scale_factor);

    // creo el Rect de dest final
    Rect dest_rect(screen_x, screen_y, scaled_w, scaled_h);

    SDL_Point center = { dest_rect.GetW() / 2, dest_rect.GetH() / 2 };
    renderer.Copy(
        sprite_sheet,
        src_rect,
        dest_rect,
        rotation_angle,
        center,
        SDL_FLIP_NONE
    );
}

void Car::set_state(float x, float y, double angle) {
    this->x = x;
    this->y = y;
    this->rotation_angle = angle;
}
