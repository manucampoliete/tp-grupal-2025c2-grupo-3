#include <SDL2pp/SDL2pp.hh>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <vector>
#include <string>

struct CollisionRect {
    float x, y, w, h;
};

class CollisionDebugRenderer {
public:
    CollisionDebugRenderer(SDL2pp::Renderer& renderer, const std::string& yaml_path)
        : renderer(renderer) {
        try {
            YAML::Node data = YAML::LoadFile(yaml_path);
            YAML::Node collisions = data["collisions"];
            for (const auto& node : collisions) {
                CollisionRect r;
                r.x = node["x"].as<float>();
                r.y = node["y"].as<float>();
                r.w = node["width"].as<float>();
                r.h = node["height"].as<float>();
                rects.push_back(r);
            }
            std::cout << "[DebugDraw] Loaded " << rects.size() << " rects from " << yaml_path << "\n";
        } catch (const std::exception& e) {
            std::cerr << "[DebugDraw] Error loading " << yaml_path << ": " << e.what() << std::endl;
        }
    }

    void draw(float cam_x, float cam_y, float zoom = 1.0f) {
        renderer.SetDrawColor(255, 0, 0, 80); // no se por que el rojo no es semi transparente

        for (const auto& r : rects) {
            SDL2pp::Rect dst(
                static_cast<int>((r.x - r.w / 2 - cam_x) * zoom),
                static_cast<int>((r.y - r.h / 2 - cam_y) * zoom),
                static_cast<int>(r.w * zoom),
                static_cast<int>(r.h * zoom)
            );
            renderer.FillRect(dst);
        }
    }

private:
    SDL2pp::Renderer& renderer;
    std::vector<CollisionRect> rects;
};
