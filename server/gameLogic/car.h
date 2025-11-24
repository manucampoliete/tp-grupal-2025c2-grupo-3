#ifndef CAR_H
#define CAR_H

#include <string>
#include <utility>

#include <box2d/box2d.h>

#include "../../../common/types/types.h"
#include "../../../common/utils/activeDirections.h"

/* #define MAX_SPEED 100.0f
#define ACC 20.0f
#define ANGULAR_SPEED 2.0f
#define TURN_FACTOR_THRESHOLD MAX_SPEED / 3
#define LINEAR_VEL_THRESHOLD 1.0f
#define ANGULAR_VEL_THRESHOLD 1.0f */

#define MAX_SPEED 10.0f
#define ACC 20.0f
#define ANGULAR_SPEED 2.0f
#define TURN_FACTOR_THRESHOLD MAX_SPEED / 3
#define LINEAR_VEL_THRESHOLD 1.0f
#define ANGULAR_VEL_THRESHOLD 1.0f


class Car {
private:
    /**
     * TODO: usar smart pointers
     */
    b2Body* body;
    ActiveDirections currentActiveDirections;

    // fisicas
    float maxSpeed;
    float acceleration;
    float angularSpeed;  // que tan rapido gira, se podria implementar con aceleracion angular para
                         // que los giros no sean bruscos
    float turnFactorThreshold;  // si la velocidad no supera este umbral entonces se penaliza la
                                // velocidad de rotacion
    float linearVelThreshold;   // si no se mueve mas rapido que esto se frena
    float angularVelThreshold;  // si no gira mas rapido que esto deja de girar

    // datos
    std::string name;
    float health;
    CarID carId;

public:
    /**
     * Constructor
     */
    explicit Car(b2Body* body, CarID carId = 0, std::string name = "Car",
                 float maxSpeed = MAX_SPEED, float acceleration = ACC,
                 float angularSpeed = ANGULAR_SPEED, float health = 100.0f):
            body(body),
            maxSpeed(maxSpeed),
            acceleration(acceleration),
            angularSpeed(angularSpeed),
            turnFactorThreshold(TURN_FACTOR_THRESHOLD),
            linearVelThreshold(LINEAR_VEL_THRESHOLD),
            angularVelThreshold(ANGULAR_VEL_THRESHOLD),
            name(std::move(name)),
            health(health),
            carId(carId) {}

    /**
     * Updates the physics of the car based on the current active directions.
     */
    void updatePhysics();

    /**
     * Updates the active directions of the car.
     */
    void updateActiveDirections(ActiveDirections activeDirections);

    void applyDamage(float impact);

    /**
     * Returns the position of the car.
     */
    b2Vec2 getPosition();

    /**
     * Returns the angle of the car in radians.
     */
    float getAngle();
    float getSpeed();
    CarID getId();
    float getHealth();

    void setCollisionLayer(uint8_t layer);
    void toggleCollisionLayer();
};

#endif
