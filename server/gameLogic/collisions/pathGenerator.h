#ifndef PATH_GENERATOR_H
#define PATH_GENERATOR_H

#include <box2d/box2d.h>
#include <vector>

#include "collisionLoader.h"
#include "../bodyData.h"
#include "collisionBits.h"

#include "pathLoader.h"

#include "../../../common/utils/pathElements.h"

#include <iostream>

#define WORLD_HEIGHT 4672.0f

class PathGenerator {
public:
    // Receives pixelsToMeters, but for now the meters <-> pixels relation is 1 to 1 (box2d may not work well with this scale)
    // worldHeight is received to be able to invert the Y axis and have the collisions where they should be (because they are generated from the image, which technically for box2d is upside down)
    static std::vector<b2Body*> GeneratePath(const Path& path, std::unique_ptr<b2World>& world, float pixelsToMeters, float worldHeight = WORLD_HEIGHT) {
        std::vector<b2Body*> bodies;
        for (const auto& element : path.elements) {
            // si es un hint (si no es un checkpoint) ignorar!
            // no contamos el start como checkpoint porque es donde spawnean los autos
            bool isCheckpoint = (element.id == CHECKPOINT_HORIZONTAL ||
                                element.id == CHECKPOINT_VERTICAL ||
                                element.id == FINISH_HORIZONTAL ||
                                element.id == FINISH_VERTICAL);
            
            if (!isCheckpoint) continue;

            float x = element.x * pixelsToMeters;            
            // Y axis is inverted
            float y = (worldHeight - element.y) * pixelsToMeters;
            
            // Hay que darle width/height según su dirección
            // Los checkpoints son de 50x22 pixeles
            float w = 0.0f;
            float h = 0.0f;
            bool isHorizontal = element.id == CHECKPOINT_HORIZONTAL ||
                                element.id == FINISH_HORIZONTAL;
            if (element.id == isHorizontal) {
                w = 22.0f;
                h = 50.0f;
            } else {
                w = 50.0f;
                h = 22.0f;
            }
            w *= pixelsToMeters;
            h *= pixelsToMeters;

            // std::cout << "[PATH_GEN] Generated chk with id: " << element.id
                // << " position (" << x << ", " << y << ")" << std::endl;


            // --- BOX2D --- //
            b2BodyDef bodyDef;
            bodyDef.type = b2_staticBody;
            bodyDef.position.Set(x, y);
            
            b2Body* body = world->CreateBody(&bodyDef);
            
            b2PolygonShape box;
            box.SetAsBox(w / 2.0f, h / 2.0f);  // SetAsBox receives "half-width" and "half-height"
            
            b2FixtureDef fixtureDef;
            fixtureDef.shape = &box;
            fixtureDef.isSensor = true;
            fixtureDef.density = 0.0f;      // This doesn't matter
            fixtureDef.friction = 0.3f;     // To slow down a bit if dragged against the wall
            fixtureDef.restitution = 0.0f;  // Bounce
            
            // Collision layer
            b2Filter filter;
            filter.categoryBits = SENSOR_LAYER;
            filter.maskBits = MASK_SENSOR;
            fixtureDef.filter = filter;

            body->CreateFixture(&fixtureDef);

            if (fixtureDef.isSensor) {
                // Body data
                auto *data = new BodyData(CHECKPOINT_SENSOR, element);
                body->GetUserData().pointer = reinterpret_cast<uintptr_t>(data); 
            }

            bodies.push_back(body);
        }
        return bodies;
    }
};

#endif  // PATH_GENERATOR_H
