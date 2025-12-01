#ifndef CAR_FACTORY_H
#define CAR_FACTORY_H

#include <memory>
#include <string>

#include <yaml-cpp/yaml.h>

#include "../../../common/types/types.h"
#include "collisions/collisionBits.h"

#include "car.h"

#include <iostream>

#define JEEP_ID 0
#define JEEP_W 28.0f * PIXELS_TO_METERS
#define JEEP_H 18.0f * PIXELS_TO_METERS

#define FERRARI_ID 1
#define FERRARI_W 40.0f * PIXELS_TO_METERS
#define FERRARI_H 20.0f * PIXELS_TO_METERS

#define BMW_ID 2
#define BMW_W 40.0f * PIXELS_TO_METERS
#define BMW_H 20.0f * PIXELS_TO_METERS

#define VW_ID 3
#define VW_W 40.0f * PIXELS_TO_METERS
#define VW_H 20.0f * PIXELS_TO_METERS

#define BRONCO_ID 4
#define BRONCO_W 38.0f * PIXELS_TO_METERS
#define BRONCO_H 22.0f * PIXELS_TO_METERS

#define F100_ID 5
#define F100_W 39.0f * PIXELS_TO_METERS
#define F100_H 20.0f * PIXELS_TO_METERS

#define MB_ID 6
#define MB_W 47.0f * PIXELS_TO_METERS
#define MB_H 18.0f * PIXELS_TO_METERS

class CarBuilder {
public:
    static Car createCar(const std::vector<CarInfo>& carsInfo, CarID carId, b2Body* body) {
        const CarInfo& carInfo = carsInfo[carId];

        b2PolygonShape box;

        float w = 0.0f;
        float h = 0.0f;

        switch (carId) {
            case JEEP_ID:
                w = JEEP_W;
                h = JEEP_H;
                break;
            case FERRARI_ID:
                w = FERRARI_W;
                h = FERRARI_H;
                break;
            case BMW_ID:
                w = BMW_W;
                h = BMW_H;
                break;
            case VW_ID:
                w = VW_W;
                h = VW_H;
                break;
            case BRONCO_ID:
                w = BRONCO_W;
                h = BRONCO_H;
                break;
            case F100_ID:
                w = F100_W;
                h = F100_H;
                break;
            case MB_ID:
                w = MB_W;
                h = MB_H;
                break;
        }

        box.SetAsBox(w/2, h/2);

        b2FixtureDef fixDef;
        fixDef.shape = &box;
        float area = w*h;
        fixDef.density = carInfo.mass/area;
        fixDef.friction = 0.3f;

        // capa de colision
        b2Filter filter;
        filter.categoryBits = CAR_LOW_LAYER;
        filter.maskBits = MASK_CAR_LOW;
        /* filter.categoryBits = CAR_HIGH_LAYER;
        filter.maskBits = MASK_CAR_HIGH; */
        fixDef.filter = filter;

        body->CreateFixture(&fixDef);

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
