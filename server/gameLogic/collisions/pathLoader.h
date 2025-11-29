#ifndef PATH_LOADER_H
#define PATH_LOADER_H

#include <yaml-cpp/yaml.h>  // For YAML parsing
#include <vector>           // For std::vector
#include <utility>          // For std::move

struct PathElement {
    int id;
    float x;
    float y;
    
    PathElement(int id, float x, float y)
        : id(id), x(x), y(y) {}

    PathElement() : id(-1), x(0), y(0) {}
};

struct Path {
    int mapId;
    std::vector<PathElement> elements;

    Path(int mapId, std::vector<PathElement>&& elements)
        : mapId(mapId), elements(std::move(elements)) {}

    Path();
};

class PathLoader {
public:
    static Path LoadPath(const std::string& pathYamlPath) {
        int mapId;
        std::vector<PathElement> path;
        
        YAML::Node config = YAML::LoadFile(pathYamlPath);
        YAML::Node segments = config["segments"];

        mapId = config["map_id"].as<int>();
        
        for (const auto& segment : segments) {
            int segId = segment["segment_start_type"].as<int>();
            float segX = segment["position"][0].as<float>();
            float segY = segment["position"][1].as<float>();
            path.emplace_back(segId, segX, segY);

            const auto& hints = segment["hints_to_next_cp"];
            if (hints && hints.IsSequence()) {
                for (const auto& hint : hints) {
                    int hintId = hint["type_id"].as<int>();
                    float hintX = hint["position"][0].as<float>();
                    float hintY = hint["position"][1].as<float>();

                    path.emplace_back(hintId, hintX, hintY);
                }
            }
        }

        return Path(mapId, std::move(path));
    }
};

#endif  // PATH_LOADER_H
