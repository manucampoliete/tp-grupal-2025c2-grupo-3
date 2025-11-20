#include "contact_listener.h"

#include <iostream>

void ContactListener::BeginContact(b2Contact* contact) {
    // Aquí puedes manejar el inicio de una colisión entre dos cuerpos
    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();

    // Ejemplo: imprimir las posiciones de los cuerpos en colisión
    b2Vec2 positionA = bodyA->GetPosition();
    b2Vec2 positionB = bodyB->GetPosition();

    (void)positionA;
    (void)positionB;

    // std::cout << "Begin Contact between Body A at (" << positionA.x << ", " << positionA.y
    //           << ") and Body B at (" << positionB.x << ", " << positionB.y << ")\n";
}

void ContactListener::EndContact(b2Contact* contact) {
    // Aquí puedes manejar el fin de una colisión entre dos cuerpos
    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();

    // Ejemplo: imprimir las posiciones de los cuerpos al finalizar la colisión
    b2Vec2 positionA = bodyA->GetPosition();
    b2Vec2 positionB = bodyB->GetPosition();

    (void)positionA;
    (void)positionB;
    // std::cout << "End Contact between Body A at (" << positionA.x << ", " << positionA.y
    //           << ") and Body B at (" << positionB.x << ", " << positionB.y << ")\n";
}

void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
    // Aquí puedes manejar la resolución de una colisión, por ejemplo, para obtener la fuerza del impacto
    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();
    (void)bodyA;
    (void)bodyB;

    // Ejemplo: imprimir la fuerza del impacto
    float force = impulse->normalImpulses[0];
    std::cout << "Furza de impacto entre Body A y Body B: " << force << "\n";
}