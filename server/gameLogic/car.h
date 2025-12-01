#ifndef CAR_H
#define CAR_H

#include <string>
#include <utility>

#include <box2d/box2d.h>

#include "../../../common/types/types.h"
#include "../../../common/utils/activeDirections.h"

#include "collisions/pathLoader.h"

/* #define MAX_SPEED 100.0f
#define ACC 20.0f
#define ANGULAR_SPEED 2.0f
#define TURN_FACTOR_THRESHOLD MAX_SPEED / 3
#define LINEAR_VEL_THRESHOLD 1.0f
#define ANGULAR_VEL_THRESHOLD 1.0f */

#define SCALE 0.05f  // modificar segun convenga, afecta a todas las constantes de velocidad

// throttle
#define MAX_SPEED 10.0f
#define ACC 8.0f

// steering
#define ANGULAR_SPEED 0.05f                      // actua sobre el torque
#define MAX_ANGULAR_SPEED 0.05f                  // ANGULAR_SPEED y MAX_ANGULAR_SPEED se pasan por yaml? o son para todos los autos?
#define STEERING_SPEED_THRESHOLD 0.1f           // velocidad minima para empezar a girar
#define MIN_STEERING_FACTOR 0.0f                // evita que el factor quede en 0 cuando hay poca velocidad
//#define TURN_FACTOR_THRESHOLD MAX_SPEED / 3

// thresholds
#define LINEAR_VEL_THRESHOLD 0.05f
#define ANGULAR_VEL_THRESHOLD 0.05f

#define VELOCITY_IMPROVEMENT_PERCENTAGE 1.5f
#define HEALTH_IMPROVEMENT_PERCENTAGE 1.05f
#define ACCELERATION_IMPROVEMENT_PERCENTAGE 1.1f
#define MASS_IMPROVEMENT_PERCENTAGE 1.1f

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
    float maxHealth;
    float currentHealth;
    CarID carId;

    void applyFriction();
    void applyThrottle();
    void applySteering();
    void applySpeedLimits();
    void applyStallPrevention();
    void handleFinishedState();

public:
    /**
     * Constructor
     */
    explicit Car(b2Body* body, CarID carId = 0, std::string name = "Car",
                 float maxSpeed = MAX_SPEED, float acceleration = ACC,
                 float angularSpeed = ANGULAR_SPEED, float maxHealth = 100.0f):
            body(body),
            maxSpeed(maxSpeed * SCALE),
            acceleration(acceleration * SCALE),
            angularSpeed(angularSpeed),
            turnFactorThreshold(maxSpeed/3),
            linearVelThreshold(LINEAR_VEL_THRESHOLD * SCALE),
            angularVelThreshold(ANGULAR_VEL_THRESHOLD),
            name(std::move(name)),
            maxHealth(maxHealth),
            currentHealth(maxHealth),
            carId(carId) {}

    /**
     * Updates the physics of the car based on the current active directions.
     * Receives whether the player finished the race or not (to apply damping and prevent it from moving)
     */
    void updatePhysics(bool finished);

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
    CarID getId();

    float getAngle();
    float getMaxSpeed() const;
    float getCurrentSpeed() const;
    float getMaxHealth() const;
    float getCurrentHealth();
    float getAcceleration() const;
    float getMass() const;

    void setPosition(const PathElement& carSpawn);
    void setCurrentHealth(float health);

    void resetSpeeds();

    void improveProperties(bool improveVelocity, bool improveHealth, bool improveAcceleration, bool improveMass);
  

    void setCollisionLayer(uint8_t layer);
    void toggleCollisionLayer();

    bool isOnBridge();

    void toggleSuperSpeed(bool superSpeed);

    b2Body* getBody() { return body; }  /** NOTE: TEMPORAL!!! */

    ~Car();
};

#endif
