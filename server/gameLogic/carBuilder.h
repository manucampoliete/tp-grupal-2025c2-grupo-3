#ifndef CAR_FACTORY_H
#define CAR_FACTORY_H

#include <memory>
#include <string>

#include <yaml-cpp/yaml.h>

#include "../../../common/types/types.h"

#include "car.h"

class CarFactory {
public:
    static Car createCar(b2Body* body, CarID carId) {
        YAML::Node config = YAML::LoadFile("config.yaml");
        YAML::Node carConfig = config["cars"][static_cast<int>(carId)];

        if (!carConfig)
            throw std::runtime_error("Car id not found in YAML: " + carId);

        float maxSpeed = carConfig["max_speed"].as<float>();
        float acceleration = carConfig["acceleration"].as<float>();
        float angularSpeed = carConfig["angular_speed"].as<float>();
        float health = carConfig["health"].as<float>();
        std::string name = carConfig["name"].as<std::string>();

        return Car(body, carId, name, maxSpeed, acceleration, angularSpeed, health);
    }
};

#endif
