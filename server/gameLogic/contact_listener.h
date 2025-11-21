#ifndef CONTACT_LISTENER_H
#define CONTACT_LISTENER_H

#include <box2d/box2d.h>
#include <set>
#include <tuple>

#include "bodyData.h"

#define IMPACT_THRESHOLD 15000.0f

class Game;

class ContactListener : public b2ContactListener {
private:
    Game* game;

    struct BodyPair {
        b2Body* bodyA;
        b2Body* bodyB;
        bool operator<(const BodyPair& other) const {
            return std::tie(bodyA, bodyB) < std::tie(other.bodyA, other.bodyB);
        }
    };

    std::set<BodyPair> alreadyHit;

public:
    explicit ContactListener(Game* game) : game(game) {}

    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;
    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
};

#endif