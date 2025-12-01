#ifndef GRAPH_GENERATOR_H
#define GRAPH_GENERATOR_H

#include <box2d/box2d.h>
#include <vector>
#include <iostream>

#include "graphLoader.h"     // tu Graph y GraphNode
#include "collisionBits.h"
#include "../bodyData.h"

#define WORLD_HEIGHT 4672.0f

// Tamaño de cada nodo sensor en pixeles
// Son circulares
#define NODE_SENSOR_RADIUS 20.0f

class GraphGenerator {
public:
    static std::vector<b2Body*> GenerateGraph(const Graph& graph, std::unique_ptr<b2World>& world, float pixelsToMeters, float worldHeight = WORLD_HEIGHT){
        std::vector<b2Body*> bodies;

        for (const auto& node : graph.nodes) {
            float x = node.x * pixelsToMeters;
            float y = (worldHeight - node.y) * pixelsToMeters;

            b2BodyDef bd;
            bd.type = b2_staticBody;
            bd.position.Set(x, y);

            b2Body* body = world->CreateBody(&bd);

            b2CircleShape shape;
            shape.m_radius = NODE_SENSOR_RADIUS * pixelsToMeters;

            b2FixtureDef fd;
            fd.shape = &shape;
            fd.isSensor = true;

            b2Filter filter;
            filter.categoryBits = SENSOR_LAYER;
            filter.maskBits = MASK_SENSOR;
            fd.filter = filter;

            body->CreateFixture(&fd);

            auto* data = new BodyData(GRAPH_NODE_SENSOR, node);
            body->GetUserData().pointer = reinterpret_cast<uintptr_t>(data);

            bodies.push_back(body);

            // Debug
            // std::cout << "[GRAPH_GENERATOR] node " << node.id
            //           << " at (" << x << "," << y << ")\n";
        }

        return bodies;
    }
};

#endif
