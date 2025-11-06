#ifndef CAR_H
#define CAR_H

#include "../common/utils/activeDirections.h"

#include <box2d/box2d.h>

class Car {
private:
    // TODO: usar smart pointers
    b2Body* body;
    ActiveDirections currentActiveDirections;
    float maxSpeed;
    float acceleration;
    float angularSpeed;  // que tan rapido gira, se podria implementar con aceleracion angular para
                          // que los giros no sean bruscos
    float turnFactorThreshold;  // si la velocidad no supera este umbral entonces se penaliza la
                                  // velocidad de rotacion
    float linearVelThreshold;   // si no se mueve mas rapido que esto se frena
    float angularVelThreshold;  // si no gira mas rapido que esto deja de girar

    void updateTurningSpeed();

public:
    explicit Car(b2Body* body):
            body(body),
            maxSpeed(10.0f),
            acceleration(2.0f),
            angularSpeed(1.0f),
            turnFactorThreshold(this->maxSpeed / 4),
            linearVelThreshold(0.1f),
            angularVelThreshold(0.1f) {}
    
    void updatePhysics();

    void updateActiveDirections(ActiveDirections activeDirections);

    b2Vec2 getPosition();

    float getAngle();
};

#endif
