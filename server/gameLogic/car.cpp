#include "car.h"

#include <utility>
#include <iostream>

#include "collisions/collisionBits.h"

#define DAMAGE_SCALE 25.0f

// Para facilitar la lectura del codigo recordar que "velocity" es un vector y "speed" es una
// magnitud

void Car::handleDestroyedState() {
    body->SetLinearDamping(3.0f);
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
    
    /* float lateralFactor = 3.0f; // asignar una variable constante
    float impulseScalar = -lateralSpeed * mass * lateralFactor;
    b2Vec2 lateralImpulse(right.x * impulseScalar, right.y * impulseScalar);
    body->ApplyLinearImpulse(lateralImpulse, body->GetWorldCenter(), true); */
    float impulseScalar = -lateralSpeed * mass * 0.5f; // reducir el lateral factor para que derrape un poco
    if (impulseScalar > 50.0f) impulseScalar = 50.0f;
    if (impulseScalar < -50.0f) impulseScalar = -50.0f;
    b2Vec2 lateralImpulse(right.x * impulseScalar, right.y * impulseScalar);
    body->ApplyLinearImpulse(lateralImpulse, body->GetWorldCenter(), true);

    float forwardSpeed = b2Dot(vel, forward);
    float dragFactor = 0.2f; // asignar una variable constante
    b2Vec2 dragForce(
        -forward.x * forwardSpeed * dragFactor,
        -forward.y * forwardSpeed * dragFactor
    );
    body->ApplyForceToCenter(dragForce, true);
}

void Car::applyThrottle() {
    float angle = body->GetAngle();
    b2Vec2 forward(std::cos(angle), std::sin(angle));
    b2Vec2 currentVel = body->GetLinearVelocity();

    if (currentActiveDirections.up) {
        b2Vec2 desiredVel(forward.x * maxSpeed, forward.y * maxSpeed);
        b2Vec2 delta = desiredVel - currentVel;

        float mass = body->GetMass();
        b2Vec2 force(delta.x * mass * acceleration, delta.y * mass * acceleration);
        body->ApplyForceToCenter(force, true);
    } else if (currentActiveDirections.down) {
        b2Vec2 desiredVel(-forward.x * maxSpeed / 2, -forward.y * maxSpeed / 2); // ahora ir para atras es mas lento que ir para adelante
        b2Vec2 delta = desiredVel - currentVel;

        float mass = body->GetMass();
        b2Vec2 force(delta.x * mass * acceleration, delta.y * mass * acceleration);
        body->ApplyForceToCenter(force, true);
    }
}

// TODO: tendria que usar un desiredAngularVel y un delta para aplicar el torque?
// igual applySpeedLimits ya se encarga de limitar eso
void Car::applySteering() {
    b2Vec2 vel = body->GetLinearVelocity();
    float speed = vel.Length();

    // si casi esta detenido no girar
    if (speed < STEERING_SPEED_THRESHOLD) {
        float currentW = body->GetAngularVelocity();
        float inertia = body->GetInertia();
        float angImpulse = -currentW * inertia * 0.5f; // angular damping
        body->ApplyAngularImpulse(angImpulse, true);
        return;
    }

    // escalado del torque segun velocidad
    // que valores debe tomar el turn factor threshold?
    float factor = speed / maxSpeed;
    if (factor < turnFactorThreshold) factor = turnFactorThreshold;
    if (factor > 1.0f) factor = 1.0f;


    float torqueToApply = ANGULAR_SPEED * factor;

    // si un auto gira el volante a la izquierda va a ir a la izquierda por mas que acelere para adelante o para atras
    // con esto se logra ese efecto, se saca el signo de la velocidad hacia adelante y se decide el steeringSign
    // se puede quitar si le resulta poco intuitivo a los jugadores
    float forwardSpeed = b2Dot(vel, b2Vec2(std::cos(body->GetAngle()), std::sin(body->GetAngle())));
    float steeringSign = (forwardSpeed >= 0.0f) ? 1.0f : -1.0f;

    if (currentActiveDirections.left) {
        body->ApplyTorque(steeringSign * torqueToApply, true);
    } else if (currentActiveDirections.right) {
        body->ApplyTorque(steeringSign * -torqueToApply, true);
    } else {
        // angular damping de nuevo, pero mas suave
        float currentW = body->GetAngularVelocity();
        float inertia = body->GetInertia();
        float angDampImpulse = -currentW * inertia * 0.05f;
        body->ApplyAngularImpulse(angDampImpulse, true);
    }
}

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
    if (std::abs(w) < ANGULAR_VEL_THRESHOLD) {
        body->SetAngularVelocity(0.0f);
        return;
    }

    if (std::abs(w) > MAX_ANGULAR_SPEED) {
        float sign = (w > 0.0f) ? 1.0f : -1.0f;
        body->SetAngularVelocity(sign * MAX_ANGULAR_SPEED);
    }
}

void Car::applyStallPrevention() {
    if(body->GetLinearVelocity().Length() < linearVelThreshold) {
        body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
    }
    if(std::abs(body->GetAngularVelocity()) < angularVelThreshold) {
        body->SetAngularVelocity(0.0f);
    }
}

void Car::updatePhysics() {
    if (currentHealth <= 0.0f) {
        handleDestroyedState();
        return;
    }

    applyFriction();
    applyThrottle();
    applySteering();
    applySpeedLimits();
    // applyStallPrevention();
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