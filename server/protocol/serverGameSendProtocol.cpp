#include "serverGameSendProtocol.h"
#include "../protocol/protocolConstants.h"

ServerGameSendProtocol::ServerGameSendProtocol(Socket& socket) : SendProtocol(socket) {}

void ServerGameSendProtocol::sendSnapshot(std::shared_ptr<Snapshot> snapshot) {
    sendU8(SEND_SNAPSHOT);
    
    // Send countdown
    sendU16(snapshot->countdown);

    // Send number of cars
    uint16_t num_cars = static_cast<uint16_t>(snapshot->cars.size());
    sendU16(num_cars);

    // Send each car's snapshot
    for (const auto& car: snapshot->cars) {
        sendU16(car.id);
        sendU32(car.x);
        sendU32(car.y);
        sendU16(car.angle);
        sendU16(car.speed);
        sendU8(car.carId);
    }
}
