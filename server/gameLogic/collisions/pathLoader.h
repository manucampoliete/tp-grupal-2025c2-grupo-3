#ifndef PATH_LOADER_H
#define PATH_LOADER_H

#include <yaml-cpp/yaml.h>  // For YAML parsing
#include <vector>           // For std::vector
#include <utility>          // For std::move

#include <ostream>

#include "../../../common/utils/pathElements.h"

#define SPAWN_UP 20
#define SPAWN_LEFT 21
#define SPAWN_RIGHT 22
#define SPAWN_DOWN 23

/* struct PathElement {
    int id;
    float x;
    float y;
    
    PathElement(int id, float x, float y)
        : id(id), x(x), y(y) {}

    PathElement() : id(-1), x(0), y(0) {}

    bool operator==(const PathElement& other) const {
        return id == other.id && x == other.x && y == other.y;
    }

    bool operator!=(const PathElement& other) const {
        return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream& os, const PathElement& p);
}; */

struct Path {
    int mapId;
    std::vector<PathElement> carSpawns;
    std::vector<PathElement> elements;

    Path(int mapId, std::vector<PathElement>&& carSpawns, std::vector<PathElement>&& elements)
        : mapId(mapId), carSpawns(std::move(carSpawns)), elements(std::move(elements)) {}

    Path() : mapId(-1) {}
};

class PathLoader {
public:
    static Path LoadPath(const std::string& pathYamlPath) {
        int mapId;
        std::vector<PathElement> carSpawns;
        std::vector<PathElement> path;

        YAML::Node config = YAML::LoadFile(pathYamlPath);
        YAML::Node spawns = config["spawns"];
        YAML::Node segments = config["segments"];

        mapId = config["map_id"].as<int>();

        for (const auto& spawn : spawns) {
            int spawnId = spawn["type_id"].as<int>();
            float spawnX = spawn["position"][0].as<float>();
            float spawnY = spawn["position"][1].as<float>();
            carSpawns.emplace_back(spawnId, spawnX, spawnY);
        }
        
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

        return Path(mapId, std::move(carSpawns), std::move(path));
    }
};

inline std::ostream& operator<<(std::ostream& os, const PathElement& p) {
    return os << "(" << p.id << ", " << p.x << ", " << p.y << ")";
}

#endif  // PATH_LOADER_H
