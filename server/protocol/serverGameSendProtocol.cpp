#include "serverGameSendProtocol.h"

#include "../../common/protocol/protocolConstants.h"

ServerGameSendProtocol::ServerGameSendProtocol(Socket& socket): SendProtocol(socket) {}

void ServerGameSendProtocol::sendSnapshot(std::shared_ptr<Snapshot> snapshot) {
    sendU8(SEND_SNAPSHOT);

    // Send countdown
    sendU32(snapshot->countdown);

    // Send number of cars
    uint8_t numCars = static_cast<uint8_t>(snapshot->cars.size());
    sendU8(numCars);

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
