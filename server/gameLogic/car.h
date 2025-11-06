#ifndef CAR_H
#define CAR_H

#include <box2d/box2d.h>

#include "../../common/utils/activeDirections.h"

class Car {
private:
    /**
     * TODO: usar smart pointers
     */
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

    /**
     * Updates the turning speed based on the current linear speed.
     */
    void updateTurningSpeed();

public:
    /**
     * Constructor
     */
    explicit Car(b2Body* body):
            body(body),
            maxSpeed(10.0f),
            acceleration(2.0f),
            angularSpeed(1.0f),
            turnFactorThreshold(this->maxSpeed / 4),
            linearVelThreshold(0.1f),
            angularVelThreshold(0.1f) {}

    /**
     * Updates the physics of the car based on the current active directions.
     */
    void updatePhysics();

    /**
     * Updates the active directions of the car.
     */
    void updateActiveDirections(ActiveDirections activeDirections);

    /**
     * Returns the position of the car.
     */
    b2Vec2 getPosition();

    /**
     * Returns the angle of the car in radians.
     */
    float getAngle();
};

#endif
