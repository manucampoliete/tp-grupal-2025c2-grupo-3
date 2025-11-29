#ifndef CAR_FACTORY_H
#define CAR_FACTORY_H

#include <memory>
#include <string>

#include <yaml-cpp/yaml.h>

#include "../../../common/types/types.h"

#include "car.h"

#include <iostream>

class CarBuilder {
public:
    static Car createCar(const std::vector<CarInfo>& carsInfo, CarID carId, b2Body* body) {
        const CarInfo& carInfo = carsInfo[carId];
        return Car(
            body, 
            carId, 
            carInfo.name, 
            carInfo.maxSpeed, 
            carInfo.acceleration, 
            carInfo.angularSpeed, 
            carInfo.health
        );
    }
};

#endif
