#include "contactListener.h"
#include "../game.h"

#include <iostream>

#include <bits/stdc++.h>

void ContactListener::BeginContact(b2Contact* contact) {
    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();

    auto* dataA = reinterpret_cast<BodyData*>(bodyA->GetUserData().pointer);
    auto* dataB = reinterpret_cast<BodyData*>(bodyB->GetUserData().pointer);

    // no hay players ni sensores involucrados
    if (!dataA && !dataB) {
        return;
    }

    // A es auto, B es sensor
    if (dataA && dataA->player && dataB && dataB->sensorId) {
        switch (dataB->sensorId) {
            case LAYER_SWITCH_SENSOR:
                dataA->player->getCar().toggleCollisionLayer();
                break;
            case CHECKPOINT_SENSOR:
                game->handleCheckpointContact(dataA->player, dataB->element);
                break;
            case GRAPH_NODE_SENSOR:
                // Graph node contact!
                break;
            default:
                break;
        }
        return;
    }

    // B es auto, A es sensor
    if (dataB && dataB->player && dataA && dataA->sensorId) {
        switch (dataA->sensorId) {
            case LAYER_SWITCH_SENSOR:
                dataB->player->getCar().toggleCollisionLayer();
                break;
            case CHECKPOINT_SENSOR:
                game->handleCheckpointContact(dataB->player, dataA->element);
                break;
            case GRAPH_NODE_SENSOR:
                // Graph node contact!
                break;
            default:
                break;
        }
        return;
    }
}

void ContactListener::EndContact(b2Contact* contact) {
    // Aquí puedes manejar el fin de una colisión entre dos cuerpos
    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();

    BodyPair pair{std::min(bodyA, bodyB), std::max(bodyA, bodyB)};
    alreadyHit.erase(pair);
}

void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {

    // Impulso normal del solver (primer punto de contacto)
    float rawImpulse = impulse->normalImpulses[0];

    // Umbrales de impulso para normalizar el impacto
    // Acelerar contra una pared constantemente se usa como referencia para evitar daño por raspaduras triviales
    const float MIN_IMPULSE = 10.0f;
    const float MAX_IMPULSE = 100.0f;

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




