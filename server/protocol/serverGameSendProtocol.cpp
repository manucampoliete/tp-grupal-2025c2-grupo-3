#include "serverGameSendProtocol.h"

#include <iostream>

#include "../../common/protocol/protocolConstants.h"

ServerGameSendProtocol::ServerGameSendProtocol(Socket& socket): SendProtocol(socket) {}

void ServerGameSendProtocol::sendSnapshot(std::shared_ptr<Snapshot> snapshot) {
    if (snapshot->type == SnapshotType::START_SIGNAL) {
        // Send start signal message
        sendU8(SEND_STARTED);
    } else {
        sendU8(SEND_SNAPSHOT);

        // std::cerr << "[PROTOCOL] Snapshot enviado: " << std::endl;

        // Send countdown
        sendU32(snapshot->countdown);

        std::cerr << "  "
                  << "Countdown: " << snapshot->countdown << std::endl;

        // Send number of cars
        uint8_t numCars = static_cast<uint8_t>(snapshot->cars.size());
        sendU8(numCars);
/* 
        std::cerr << "  "
                  << "numCars: " << numCars << std::endl; */

        // Send each car's snapshot
        for (const auto& car: snapshot->cars) {
            sendU16(car.id);
            sendU32(car.x);
            sendU32(car.y);
            sendU16(car.angle);
            sendU16(car.speed);
            sendU8(car.carId);
/*             std::cerr << "  "
                      << "Car id: " << car.id << std::endl;
            std::cerr << "  "
                      << "X: " << car.x << std::endl;
            std::cerr << "  "
                      << "Y: " << car.y << std::endl;
            std::cerr << "  "
                      << "Angle: " << car.angle << std::endl;
            std::cerr << "  "
                      << "Speed: " << car.speed << std::endl;
            std::cerr << "  "
                      << "carId: " << car.carId << std::endl; */
        }
    }
}
