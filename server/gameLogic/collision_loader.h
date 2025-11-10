#include <box2d/box2d.h>
#include <yaml-cpp/yaml.h>
#include <vector>
#include <iostream>

class CollisionLoader {
public:
// recibe pixelsToMeters, pero por el momento la relacion metros <-> pixeles es 1 a 1 (box2d puede andar mal con esta escala)
// worldHeight se recibe para poder invertir el eje Y y que las colisiones estén donde tienen que estar (porque se generan de la imagen, que tecnicamente para box2d está al revés)
static std::vector<b2Body*> LoadCollisions(const std::string& yamlPath, b2World* world, float pixelsToMeters = 1.0f, float worldHeight = 0.0f) {
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
            fixtureDef.density = 0.0f;      // esto no importa
            fixtureDef.friction = 0.3f;     // para que se frene un poco si se arrastra contra la pared
            fixtureDef.restitution = 0.1f;  // para rebotar un poco contra las paredes
            
            body->CreateFixture(&fixtureDef);
            
            collisionBodies.push_back(body);
        }        
    } catch (const YAML::Exception& e) {
        std::cerr << "LoadCollisions Error: " << e.what() << std::endl;
    }
    
    return collisionBodies;
    }
};