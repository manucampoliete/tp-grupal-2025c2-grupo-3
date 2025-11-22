#include "car.h"

#include <utility>
#include <iostream>

#define DAMAGE_SCALE 50.0f

// Para facilitar la lectura del codigo recordar que "velocity" es un vector y "speed" es una
// magnitud

/**
 * TODO: modularizar
 */
void Car::updatePhysics() {
    if(health <= 0.0f) {
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
    std::cout << "///////////////////////" << std::endl;
    std::cout << "[CAR] Starting health: " << health << std::endl;
    health -= impact * DAMAGE_SCALE; 
    if (health < 0.0f)
        health = 0.0f;
    std::cout << "[CAR] Applied damage: " << impact * DAMAGE_SCALE << std::endl;
    std::cout << "[CAR] Remaining health: " << health << std::endl;
}

b2Vec2 Car::getPosition() { return body->GetPosition(); }

float Car::getAngle() { return body->GetAngle(); }

float Car::getSpeed() { return body->GetLinearVelocity().Length(); }

CarID Car::getId() { return carId; }

float Car::getHealth() { return health; }
