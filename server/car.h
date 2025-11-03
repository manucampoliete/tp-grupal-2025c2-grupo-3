#ifndef CAR_H
#define CAR_H

// #include "../common/utils/vector_2d.h"
#include "types.h"
#include "commands/command.h"
#include "../common/utils/active_directions.h"
#include <box2d/box2d.h>

class Car {
private:
    // TODO: usar smart pointers
    b2Body* body;
    ActiveDirections current_active_directions;
    float max_speed;
    float acceleration;
    float angular_speed;    // que tan rapido gira, se podria implementar con aceleracion angular para que los giros no sean bruscos
    float turn_factor_threshold;    // si la velocidad no supera este umbral entonces se penaliza la velocidad de rotacion
    float linear_vel_threshold;     // si no se mueve mas rapido que esto se frena
    float angular_vel_threshold;    // si no gira mas rapido que esto deja de girar

    void update_turning_speed();

public:
    Car(b2Body* body) : body(body), max_speed(10.0f), acceleration(2.0f), angular_speed(1.0f), turn_factor_threshold(this->max_speed/4), linear_vel_threshold(0.1f), angular_vel_threshold(0.1f) {}

    void update_physics();

    void update_active_directions(ActiveDirections active_directions);

    b2Vec2 get_position() {return body->GetPosition();}

    float get_angle() {return body->GetAngle();}
};

#endif