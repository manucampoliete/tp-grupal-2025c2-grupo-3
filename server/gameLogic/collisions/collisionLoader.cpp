#include "collisionLoader.h" 

#include "../bodyData.h"

std::vector<b2Body*> CollisionLoader::LoadCollisions(const std::string& yamlPath, std::unique_ptr<b2World>& world, float pixelsToMeters, float worldHeight, uint8_t layer, bool isSensor)
{
    std::vector<b2Body*> collisionBodies;
    try {
        YAML::Node config = YAML::LoadFile(yamlPath);
        YAML::Node collisions = config["collisions"];
        
        // si no se pasó worldHeight igual agarro la que esta en el yaml
        if (worldHeight == 0.0f && config["map"]["height"]) {
            worldHeight = config["map"]["height"].as<float>();
        }
        
        for (const auto& collision : collisions) {
            float x = collision["x"].as<float>() / pixelsToMeters;
            float y_pixels = collision["y"].as<float>();
            
            // se invierte Y
            float y = (worldHeight > 0.0f) 
                ? (worldHeight - y_pixels) / pixelsToMeters 
                : y_pixels / pixelsToMeters; 
            
            float width = collision["width"].as<float>() / pixelsToMeters;
            float height = collision["height"].as<float>() / pixelsToMeters;

            // BOX2D //
            b2BodyDef bodyDef;
            bodyDef.type = b2_staticBody;
            bodyDef.position.Set(x, y);
            
            b2Body* body = world->CreateBody(&bodyDef);
            
            b2PolygonShape box;
            box.SetAsBox(width / 2.0f, height / 2.0f);  // SetAsBox recibe "half-width" y "half-height"
            
            b2FixtureDef fixtureDef;
            fixtureDef.shape = &box;
            fixtureDef.isSensor = isSensor;
            fixtureDef.density = 0.0f;      // esto no importa
            fixtureDef.friction = 0.3f;     // para que se frene un poco si se arrastra contra la pared
            fixtureDef.restitution = 0.0f;  // rebote
            
            // capa de colision
            b2Filter filter;
            filter.categoryBits = layer;
            filter.maskBits = layer == WALL_LOW_LAYER ? MASK_WALL_LOW :
                              layer == WALL_HIGH_LAYER ? MASK_WALL_HIGH :
                              layer == SENSOR_LAYER ? MASK_SENSOR : 0;
            fixtureDef.filter = filter;

            body->CreateFixture(&fixtureDef);

            if(isSensor) {
                // datos del cuerpo
                auto* data = new BodyData();
                data->player = nullptr;
                data->sensorId = LAYER_SWITCH_SENSOR; // por ahora solo hay un tipo de sensor
                body->GetUserData().pointer = reinterpret_cast<uintptr_t>(data);
            }
            
            collisionBodies.push_back(body);
        }        
    } catch (const YAML::Exception& e) {
        std::cerr << "LoadCollisions Error: " << e.what() << std::endl;
    }
    
    return collisionBodies;
}
