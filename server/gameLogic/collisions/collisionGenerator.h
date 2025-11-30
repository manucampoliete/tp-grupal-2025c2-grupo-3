#ifndef COLLISION_GENERATOR_H
#define COLLISION_GENERATOR_H

#include <box2d/box2d.h>
#include <vector>

#include "collisionLoader.h"
#include "../bodyData.h"
#include "collisionBits.h"

class CollisionGenerator {
public:
    // Receives pixelsToMeters, but for now the meters <-> pixels relation is 1 to 1 (box2d may not work well with this scale)
    // worldHeight is received to be able to invert the Y axis and have the collisions where they should be (because they are generated from the image, which technically for box2d is upside down)
    static std::vector<b2Body*> GenerateCollisions(const CollisionMap& collisionMap, std::unique_ptr<b2World>& world, float pixelsToMeters, float worldHeight, uint8_t layer, int sensorId = NOT_A_SENSOR) {
        // If worldHeight is not passed, I take the one from the collision map
        if (worldHeight == 0.0f) {
            worldHeight = collisionMap.mapHeight;
        }
        
        std::vector<b2Body*> bodies;
        for (const auto& collision : collisionMap.collisionBoxes) {
            float x = collision.x * pixelsToMeters;
            float y_pixels = collision.y;
            
            // Y axis is inverted
            float y = (worldHeight > 0.0f) 
                ? (worldHeight - y_pixels) * pixelsToMeters 
                : y_pixels * pixelsToMeters; 
            
            float w = collision.w * pixelsToMeters;
            float h = collision.h * pixelsToMeters;

            // --- BOX2D --- //
            b2BodyDef bodyDef;
            bodyDef.type = b2_staticBody;
            bodyDef.position.Set(x, y);
            
            b2Body* body = world->CreateBody(&bodyDef);
            
            b2PolygonShape box;
            box.SetAsBox(w / 2.0f, h / 2.0f);  // SetAsBox receives "half-width" and "half-height"
            
            b2FixtureDef fixtureDef;
            fixtureDef.shape = &box;
            fixtureDef.isSensor = sensorId != NOT_A_SENSOR;
            fixtureDef.density = 0.0f;      // This doesn't matter
            fixtureDef.friction = 0.3f;     // To slow down a bit if dragged against the wall
            fixtureDef.restitution = 0.0f;  // Bounce
            
            // Collision layer
            b2Filter filter;
            filter.categoryBits = layer;
            filter.maskBits = layer == WALL_LOW_LAYER ? MASK_WALL_LOW :
                                layer == WALL_HIGH_LAYER ? MASK_WALL_HIGH :
                                layer == SENSOR_LAYER ? MASK_SENSOR : 0;
            fixtureDef.filter = filter;

            body->CreateFixture(&fixtureDef);

            if (fixtureDef.isSensor) {
                // Body data
                auto *data = new BodyData(sensorId);  // For now there is only one type of sensor
                body->GetUserData().pointer = reinterpret_cast<uintptr_t>(data);
            }

            bodies.push_back(body);
        }
        return bodies;
    }
};

#endif  // COLLISION_GENERATOR_H
