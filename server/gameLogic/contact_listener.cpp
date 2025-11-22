#include "contact_listener.h"
#include "game.h"

#include <iostream>

#include <bits/stdc++.h>

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

    BodyPair pair{std::min(bodyA, bodyB), std::max(bodyA, bodyB)};
    alreadyHit.erase(pair);
}

/* void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
    // Aquí puedes manejar la resolución de una colisión, por ejemplo, para obtener la fuerza del impacto
    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();
    
    // En cada frame de una "raspadura" se llama varias veces a PostSolve
    // Box2D puede llegar a interpretar el mismo choque como (body1, body2) y (body2, body1) en frames contiguos 
    // Esto nos asegura de que va a interpretar el choque siempre de la misma forma
    BodyPair pair{std::min(bodyA, bodyB), std::max(bodyA, bodyB)};

    float impact = impulse->normalImpulses[0];

    if (impact > IMPACT_THRESHOLD && !alreadyHit.count(pair)) {
        alreadyHit.insert(pair);
        std::cout << "Impact: " << impact << "\n";
        //applyDamage(bodyA, bodyB, impact);
        // en realidad no va a ser un applyDamage sino mas bien un handle collision
        // que ademas de aplicar el daño va a hacer un broadcast de la colisión
        
        auto* dataA = reinterpret_cast<BodyData*>(bodyA->GetUserData().pointer);
        auto* dataB = reinterpret_cast<BodyData*>(bodyB->GetUserData().pointer);

        // alguno de los involucrados puede no ser un player (auto contra pared)
        if(dataA && dataA->player) {
            Player* playerA = dataA->player;
            game->handleCollision(playerA, impact);
        }

        if(dataB && dataB->player) {
            Player* playerB = dataB->player;
            game->handleCollision(playerB, impact);
        }
    }
} */

void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {

    // Impulso normal del solver (primer punto de contacto)
    float rawImpulse = impulse->normalImpulses[0];

    const float MIN_IMPULSE = 16000.0f;
    const float MAX_IMPULSE = 40000.0f;

    if (rawImpulse < MIN_IMPULSE) {
        return;   // ignorar raspadura
    }

    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();

    // Los pares se crean con std::min y std::max para evitar pares que tienen los mismos
    // curpos pero en distinto orden (a veces box2d hace eso)
    BodyPair pair{ std::min(bodyA, bodyB), std::max(bodyA, bodyB) };

    // Si ya se procesó este par, no se hace nada
    if (alreadyHit.count(pair)) {
        return;
    }

    alreadyHit.insert(pair);

    // Normalizar (el cliente recibe la intensidad normalizada)
    float normalized = (rawImpulse - MIN_IMPULSE) / (MAX_IMPULSE - MIN_IMPULSE);
    normalized = std::clamp(normalized, 0.0f, 1.0f);

    // Cada b2Body tiene un Player* en su userData (si es un auto de un jugador)
    auto* dataA = reinterpret_cast<BodyData*>(bodyA->GetUserData().pointer);
    auto* dataB = reinterpret_cast<BodyData*>(bodyB->GetUserData().pointer);

    if (dataA && dataA->player) {
        game->handleCollision(dataA->player, normalized);
    }
    if (dataB && dataB->player) {
        game->handleCollision(dataB->player, normalized);
    }
}




