#ifndef COLLISION_LOADER_H
#define COLLISION_LOADER_H

#include <yaml-cpp/yaml.h>  // For YAML parsing
#include <vector>           // For std::vector
#include <utility>          // For std::move

struct CollisionBox {
    const float x;
    const float y;
    const float w;
    const float h;
    
    CollisionBox(float x, float y, float w, float h)
        : x(x), y(y), w(w), h(h) {}
};

struct CollisionMap {
    const uint8_t mapWidth;
    const uint8_t mapHeight;
    const uint8_t blockSize;
    const std::vector<CollisionBox> collisionBoxes;

    CollisionMap(uint8_t width, uint8_t height, uint8_t block, std::vector<CollisionBox>&& boxes)
        : mapWidth(width), mapHeight(height), blockSize(block), collisionBoxes(std::move(boxes)) {}
};

class CollisionLoader {
public:
    static CollisionMap LoadCollisions(const std::string& collisionsYamlPath) {
        uint8_t mapWidth, mapHeight, blockSize;
        std::vector<CollisionBox> boxes;
        
        YAML::Node config = YAML::LoadFile(collisionsYamlPath);
        YAML::Node collisions = config["collisions"];
            
        mapWidth = static_cast<uint8_t>(config["map"]["width"].as<int>());
        mapHeight = static_cast<uint8_t>(config["map"]["height"].as<int>());
        blockSize = static_cast<uint8_t>(config["map"]["block_size"].as<int>());
        
        for (const auto& collision : collisions) {
            float x = collision["x"].as<float>();
            float y = collision["y"].as<float>();
            float w = collision["width"].as<float>();
            float h = collision["height"].as<float>();
            boxes.emplace_back(x, y, w, h);
        }

        return CollisionMap(mapWidth, mapHeight, blockSize, std::move(boxes));
    }
};

#endif  // COLLISION_LOADER_H
