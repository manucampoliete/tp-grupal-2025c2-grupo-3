#include "configLoader.h"
#include <yaml-cpp/yaml.h>

Config ConfigLoader::Load(const std::string& configYamlPath) {
    YAML::Node root = YAML::LoadFile(configYamlPath);

    // Parse cars
    std::vector<CarInfo> carsInfo;
    YAML::Node carsNode = root["cars"];  // Using const& crashes

    for (auto it = carsNode.begin(); it != carsNode.end(); ++it) {
        CarID id = static_cast<CarID>(it->first.as<int>());
        YAML::Node car = it->second;  // Using const& crashes

        float maxSpeed      = car["max_speed"].as<float>();
        float acceleration  = car["acceleration"].as<float>();
        float angularSpeed  = car["angular_speed"].as<float>();
        float mass          = car["mass"].as<float>();
        std::string name    = car["name"].as<std::string>();
        float health        = car["health"].as<float>();

        carsInfo.emplace_back(
            id,
            maxSpeed,
            acceleration,
            angularSpeed,
            mass,
            name,
            health
        );
    }

    // Parse timers
    YAML::Node timersNode = root["game_phases_timers"];  // Using const& crashes

    GamePhasesTimers timers(
        static_cast<uint8_t>(timersNode["countdown"].as<int>()),
        static_cast<uint8_t>(timersNode["racing"].as<int>()),
        static_cast<uint8_t>(timersNode["showing_stats"].as<int>()),
        static_cast<uint8_t>(timersNode["modifying_car"].as<int>()),
        static_cast<uint8_t>(timersNode["game_end"].as<int>())
    );

    // Parse other fields
    uint8_t numberOfRaces = static_cast<uint8_t>(root["number_of_races"].as<int>());
    uint8_t rateLoop      = static_cast<uint8_t>(root["rate_loop"].as<int>());

    return Config(std::move(carsInfo), timers, numberOfRaces, rateLoop);
}
