#include "car.h"

// Para facilitar la lectura del codigo recordar que "velocity" es un vector y "speed" es una magnitud

void Car::update_active_directions(ActiveDirections active_directions) {
    current_active_directions = active_directions;
}

// TODO: modularizar
void Car::update_physics() {
    // manejo del giro
    b2Vec2 vel = body->GetLinearVelocity();
    float speed = vel.Length();
    float turn_factor = speed > turn_factor_threshold? 1 : speed/turn_factor_threshold;

    if (current_active_directions.left) body->SetAngularVelocity(angular_speed * turn_factor);
    else if (current_active_directions.right) body->SetAngularVelocity(-angular_speed * turn_factor);
    else body->SetAngularVelocity(0);

    // para que avance en la direccion en la que mira
    float angle = body->GetAngle();
    b2Vec2 forward(cos(angle), sin(angle));
    // b2Vec2 vel = body->GetLinearVelocity();
    vel = body->GetLinearVelocity();

    // para que el auto no rote sobre su propio eje cuando se suelta el acelerador y se gira
    float velForwardProjection = b2Dot(vel, forward);   //me da la magnitud de la velocidad en la direccion en la que mira el auto
    b2Vec2 newVel(forward.x * velForwardProjection, forward.y * velForwardProjection);
    body->SetLinearVelocity(newVel);

    if (current_active_directions.up) {
        b2Vec2 desiredVel(forward.x * max_speed, forward.y * max_speed);  //recordar que la velocity es vectorial y speed es una magnitud
        b2Vec2 currentVel = body->GetLinearVelocity();

        b2Vec2 force((desiredVel.x - currentVel.x) * body->GetMass() * acceleration, (desiredVel.y - currentVel.y) * body->GetMass() * acceleration);
        body->ApplyForceToCenter(force, true);
    }
    if (current_active_directions.down) {
        b2Vec2 desiredVel(-forward.x * max_speed, -forward.y * max_speed);
        b2Vec2 currentVel = body->GetLinearVelocity();
        
        b2Vec2 force((desiredVel.x - currentVel.x) * body->GetMass() * acceleration, (desiredVel.y - currentVel.y) * body->GetMass() * acceleration);
        body->ApplyForceToCenter(force, true);
    }
    // body->SetLinearVelocity(vel);

    // b2Vec2 vel = body->GetLinearVelocity();
    vel = body->GetLinearVelocity();
    // float speed = vel.Length();
    speed = vel.Length();
    if(speed > max_speed) {
        vel.x = vel.x * max_speed / speed;
        vel.y = vel.y * max_speed / speed;
        body->SetLinearVelocity(vel);
    }

    // limite sobre la velocidad minima (lineal y rotacional)
    b2Vec2 null_linear_vel(0,0);
    if(body->GetLinearVelocity().Length() < linear_vel_threshold) body->SetLinearVelocity(null_linear_vel);
    if(std::abs(body->GetAngularVelocity()) < angular_vel_threshold) body->SetAngularVelocity(0);
}