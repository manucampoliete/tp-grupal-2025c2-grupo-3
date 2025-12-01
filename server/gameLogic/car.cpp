#include "car.h"

#include <utility>
#include <iostream>

#include "collisions/collisionBits.h"
#include "bodyData.h"

// Para facilitar la lectura del codigo recordar que "velocity" es un vector y "speed" es una
// magnitud

void Car::handleFinishedState() {
    body->SetLinearDamping(1.0f);
    body->SetAngularDamping(6.0f);
}

void Car::applyFriction() {
    b2Vec2 vel = body->GetLinearVelocity();
    float angle = body->GetAngle();

    b2Vec2 forward(std::cos(angle), std::sin(angle));
    b2Vec2 right(-forward.y, forward.x);

    // saca la componente lateral de la velocidad
    float lateralSpeed = b2Dot(vel, right);

    float mass = body->GetMass();
    
    float impulseScalar = -lateralSpeed * mass * 0.5f; // reducir el lateral factor para que derrape un poco
    if (impulseScalar > 50.0f) impulseScalar = 50.0f;
    if (impulseScalar < -50.0f) impulseScalar = -50.0f;
    b2Vec2 lateralImpulse(right.x * impulseScalar, right.y * impulseScalar);
    body->ApplyLinearImpulse(lateralImpulse, body->GetWorldCenter(), true);

    float forwardSpeed = b2Dot(vel, forward);
    float drag = -forwardSpeed * std::abs(forwardSpeed) * 0.05f;
    b2Vec2 dragForce = drag * forward;
    body->ApplyForceToCenter(dragForce, true);
}

void Car::applyThrottle() {
    float angle = body->GetAngle();
    b2Vec2 forward(std::cos(angle), std::sin(angle));
    b2Vec2 currentVel = body->GetLinearVelocity();
    float mass = body->GetMass();
    float targetSpeed = 0.0f;

    if (currentActiveDirections.up) {
        targetSpeed = maxSpeed;
    } else if (currentActiveDirections.down) {
        targetSpeed = -maxSpeed * 0.75f;
    }

    float currentForward = b2Dot(currentVel, forward);
    float deltaSpeed = targetSpeed - currentForward;

    float forceMag = acceleration * mass * deltaSpeed;

    b2Vec2 force(forward.x * forceMag, forward.y * forceMag);
    body->ApplyForceToCenter(force, true);
}

// TODO: tendria que usar un desiredAngularVel y un delta para aplicar el torque?
// igual applySpeedLimits ya se encarga de limitar eso
void Car::applySteering() {
    b2Vec2 vel = body->GetLinearVelocity();
    float speed = vel.Length();

    // si casi esta detenido no girar
    if (speed < STEERING_SPEED_THRESHOLD) {
        return;
    }

    float angle = body->GetAngle();
    b2Vec2 forward(std::cos(angle), std::sin(angle));

    float forwardSpeed = b2Dot(vel, forward);
    float steeringSign = (forwardSpeed >= 0) ? 1.0f : -1.0f;

    float turn = 0.0f;
    if (currentActiveDirections.left)
        turn = 1.0f;
    else if (currentActiveDirections.right)
        turn = -1.0f;
    if (turn == 0.0f) return;

    float speedFactor = std::clamp(speed/maxSpeed, 0.2f, 1.0f);

    float torque = angularSpeed * steeringSign * turn * speedFactor * body->GetMass();

    body->ApplyTorque(torque, true);
}

// aplica limites tanto inferiores como superiores a las velocidades
void Car::applySpeedLimits() {
    // velocidad lineal
    b2Vec2 vel = body->GetLinearVelocity();
    float speed = vel.Length();

    if (speed > maxSpeed) {
        float scale = maxSpeed / speed;
        b2Vec2 newVel(vel.x * scale, vel.y * scale);
        body->SetLinearVelocity(newVel);
    }

    // velocidad angular
    float w = body->GetAngularVelocity();

    if (std::abs(w) > MAX_ANGULAR_SPEED) {
        float sign = (w > 0.0f) ? 1.0f : -1.0f;
        body->SetAngularVelocity(sign * MAX_ANGULAR_SPEED);
    }
    if (std::abs(w) < ANGULAR_VEL_THRESHOLD) {
        body->SetAngularVelocity(0.0f);
        return;
    }
}

void Car::updatePhysics(bool finished) {
    if (finished) {
        handleFinishedState();
        return;
    }

    applyFriction();
    applyThrottle();
    applySteering();
    applySpeedLimits();
}

void Car::updateActiveDirections(ActiveDirections activeDirections) {
    currentActiveDirections = activeDirections;
}

void Car::applyDamage(float impact) {
    // el impacto ya viene normalizado
    currentHealth -= impact * DAMAGE_SCALE; 
    if (currentHealth < 0.0f)
        currentHealth = 0.0f;
}

b2Vec2 Car::getPosition() { return body->GetPosition(); }

float Car::getAngle() { return body->GetAngle(); }

float Car::getCurrentSpeed() const { return body->GetLinearVelocity().Length(); }

CarID Car::getId() { return carId; }

float Car::getMaxHealth() const { return maxHealth; }

float Car::getMaxSpeed() const { return maxSpeed; } 

void Car::setCurrentHealth(float health) { currentHealth = health; }

void Car::setPosition(const PathElement& carSpawn) {
    float x = carSpawn.x * PIXELS_TO_METERS;
    float y = (WORLD_HEIGHT - carSpawn.y) * PIXELS_TO_METERS;
    switch (carSpawn.id) {
        case SPAWN_UP:
            body->SetTransform(b2Vec2(x, y), 90*DEGTORAD);
            break;
        case SPAWN_DOWN:
            body->SetTransform(b2Vec2(x, y), 270*DEGTORAD);
            break;
        case SPAWN_LEFT:
            body->SetTransform(b2Vec2(x, y), 180*DEGTORAD);
            break;
        case SPAWN_RIGHT:
            body->SetTransform(b2Vec2(x, y), 0*DEGTORAD);
            break;
    }
}

void Car::improveProperties(bool improveVelocity, bool improveHealth, bool improveAcceleration, bool improveMass) {
    if (improveVelocity) {
        maxSpeed *= VELOCITY_IMPROVEMENT_PERCENTAGE;
    }
    if (improveHealth) {
        maxHealth *= HEALTH_IMPROVEMENT_PERCENTAGE;
    }
    if (improveAcceleration) {
        acceleration *= ACCELERATION_IMPROVEMENT_PERCENTAGE;
    }
    if (improveMass) {
        b2MassData md;
        body->GetMassData(&md);
        md.I *= MASS_IMPROVEMENT_PERCENTAGE;
        md.mass *= MASS_IMPROVEMENT_PERCENTAGE;
        body->SetMassData(&md);
    }
}

float Car::getCurrentHealth() { return currentHealth; }

void Car::setCollisionLayer(uint8_t layer) {
    b2Filter filter = body->GetFixtureList()->GetFilterData();
    filter.categoryBits = layer;
    filter.maskBits = (layer == CAR_LOW_LAYER) ? MASK_CAR_LOW : MASK_CAR_HIGH;
    body->GetFixtureList()->SetFilterData(filter);
}

void Car::toggleCollisionLayer() {
    b2Filter filter = body->GetFixtureList()->GetFilterData();
    if (filter.categoryBits == CAR_LOW_LAYER) {
        filter.categoryBits = CAR_HIGH_LAYER;
        filter.maskBits = MASK_CAR_HIGH;
    } else {
        filter.categoryBits = CAR_LOW_LAYER;
        filter.maskBits = MASK_CAR_LOW;
    }
    body->GetFixtureList()->SetFilterData(filter);
}

bool Car::isOnBridge() {
    b2Filter filter = body->GetFixtureList()->GetFilterData();
    return (filter.categoryBits == CAR_HIGH_LAYER);
}

float Car::getAcceleration() const {
    return acceleration;
}

float Car::getMass() const {
    b2MassData md;
    body->GetMassData(&md);
    return md.mass;
}

void Car::resetSpeeds() {
    body->SetLinearVelocity(b2Vec2_zero);
    body->SetAngularVelocity(0.0f);
}

void Car::toggleSuperSpeed(bool superSpeed) {
    if (superSpeed) {
        maxSpeed /= SUPERSPEED_SCALE;
        acceleration /= SUPERSPEED_SCALE;
    } else {
        maxSpeed *= SUPERSPEED_SCALE;
        acceleration *= SUPERSPEED_SCALE;
    }
}


Car::~Car() {
    // This line causes invalid reads (see ContactListener)
    // I prefer to leak the BodyData for now (16 bytes per body)

    // delete reinterpret_cast<BodyData*>(body->GetUserData().pointer);

    /**
     * TODO: solve memory leak without causing invalid reads
     */
}
