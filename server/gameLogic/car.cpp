#include "car.h"

#include <utility>
#include <iostream>

#include "collisions/collisionBits.h"

#define DAMAGE_SCALE 50.0f

// Para facilitar la lectura del codigo recordar que "velocity" es un vector y "speed" es una
// magnitud

/**
 * TODO: modularizar
 */
void Car::updatePhysics() {
    if(currentHealth <= 0.0f) {
        // el auto esta destruido, se frena rapido y no se puede mover
        body->SetLinearDamping(2.5f);
        body->SetAngularDamping(5.0f);
        return;
    }

    // manejo del giro
    b2Vec2 vel = body->GetLinearVelocity();
    float speed = vel.Length();
    float turnFactor = speed > turnFactorThreshold ? 1 : speed / MAX_SPEED; // cambiar para que sea mas realista

    if (currentActiveDirections.left)
        body->SetAngularVelocity(angularSpeed * turnFactor);
    else if (currentActiveDirections.right)
        body->SetAngularVelocity(-angularSpeed * turnFactor);
    else
        body->SetAngularVelocity(0);

    // para que avance en la direccion en la que mira
    float angle = body->GetAngle();
    b2Vec2 forward(cos(angle), sin(angle));
    // b2Vec2 vel = body->GetLinearVelocity();
    vel = body->GetLinearVelocity();

    // para que el auto no rote sobre su propio eje cuando se suelta el acelerador y se gira
    float velForwardProjection = b2Dot(
            vel,
            forward);  // me da la magnitud de la velocidad en la direccion en la que mira el auto
    b2Vec2 newVel(forward.x * velForwardProjection, forward.y * velForwardProjection);
    body->SetLinearVelocity(newVel);

    if (currentActiveDirections.up) {
        b2Vec2 desiredVel(forward.x * maxSpeed,
                          forward.y * maxSpeed);  // recordar que la velocity es vectorial y speed
                                                  // es una magnitud
        b2Vec2 currentVel = body->GetLinearVelocity();

        b2Vec2 force((desiredVel.x - currentVel.x) * body->GetMass() * acceleration,
                     (desiredVel.y - currentVel.y) * body->GetMass() * acceleration);
        body->ApplyForceToCenter(force, true);
    }
    if (currentActiveDirections.down) {
        b2Vec2 desiredVel(-forward.x * maxSpeed, -forward.y * maxSpeed);
        b2Vec2 currentVel = body->GetLinearVelocity();

        b2Vec2 force((desiredVel.x - currentVel.x) * body->GetMass() * acceleration,
                     (desiredVel.y - currentVel.y) * body->GetMass() * acceleration);
        body->ApplyForceToCenter(force, true);
    }
    // body->SetLinearVelocity(vel);

    // b2Vec2 vel = body->GetLinearVelocity();
    vel = body->GetLinearVelocity();
    // float speed = vel.Length();
    speed = vel.Length();
    if (speed > maxSpeed) {
        vel.x = vel.x * maxSpeed / speed;
        vel.y = vel.y * maxSpeed / speed;
        body->SetLinearVelocity(vel);
    }

    // limite sobre la velocidad minima (lineal y rotacional)
    b2Vec2 nullLinearVel(0, 0);
    if (body->GetLinearVelocity().Length() < linearVelThreshold)
        body->SetLinearVelocity(nullLinearVel);
    if (std::abs(body->GetAngularVelocity()) < angularVelThreshold)
        body->SetAngularVelocity(0);
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

void Car::improveProperties(bool improveVelocity, bool improveHealth) {
    if (improveVelocity) {
        maxSpeed *= VELOCITY_IMPROVEMENT_PERCENTAGE;
    }
    if (improveHealth) {
        maxHealth *= HEALTH_IMPROVEMENT_PERCENTAGE;
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