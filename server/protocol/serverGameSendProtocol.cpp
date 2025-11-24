#include "serverGameSendProtocol.h"

#include <iostream>

#include "../../common/protocol/protocolConstants.h"

ServerGameSendProtocol::ServerGameSendProtocol(Socket& socket): SendProtocol(socket) {}

void ServerGameSendProtocol::sendRaceSnapshot(std::shared_ptr<Snapshot> snapshot) {
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

void ServerGameSendProtocol::sendCollisionSnapshot(const Snapshot::CollisionData& collision) {
    sendU16(collision.playerId);
    // convertir intensidad float (0.0-1.0) a uint8_t (0-255)
    uint8_t intensity = static_cast<uint8_t>(collision.intensity * 255.0f);
    sendU8(intensity);
    sendU32(collision.x);
    sendU32(collision.y);
}

void ServerGameSendProtocol::sendSnapshot(std::shared_ptr<Snapshot> snapshot) {
    sendU8(snapshot->type);
    switch (snapshot->type) {
        case SEND_STARTED:
            // nada
            break;
        case MSG_RACE_START:
            // nada
            break;
        case MSG_COUNTDOWN:
            sendU8(static_cast<uint8_t>(snapshot->countdown));
            break;
        case SEND_RACE_SNAPSHOT:
            sendRaceSnapshot(snapshot);
            break;
        case MSG_COLLISION:
            sendCollisionSnapshot(snapshot->collisionData);
            break;
        case MSG_RACE_END:  // estoy seria para mostrar las estadisticas
        case MSG_MOD_PHASE:
        case MSG_GAME_END:
            // no implementado
            break;
    }
}

// void ServerGameSendProtocol::sendSnapshot(std::shared_ptr<Snapshot> snapshot) {
//     if (snapshot->type == SnapshotType::START_SIGNAL) {
//         // Send start signal message
//         sendU8(SEND_STARTED);
//     } else {
//         sendU8(SEND_RACE_SNAPSHOT);

//         // std::cerr << "[PROTOCOL] Snapshot enviado: " << std::endl;

//         // Send countdown
//         sendU32(snapshot->countdown);

//         std::cerr << "  "
//                   << "Countdown: " << snapshot->countdown << std::endl;

//         // Send number of cars
//         uint8_t numCars = static_cast<uint8_t>(snapshot->cars.size());
//         sendU8(numCars);
// /* 
//         std::cerr << "  "
//                   << "numCars: " << numCars << std::endl; */

//         // Send each car's snapshot
//         for (const auto& car: snapshot->cars) {
//             sendU16(car.id);
//             sendU32(car.x);
//             sendU32(car.y);
//             sendU16(car.angle);
//             sendU16(car.speed);
//             sendU8(car.carId);
// /*             std::cerr << "  "
//                       << "Car id: " << car.id << std::endl;
//             std::cerr << "  "
//                       << "X: " << car.x << std::endl;
//             std::cerr << "  "
//                       << "Y: " << car.y << std::endl;
//             std::cerr << "  "
//                       << "Angle: " << car.angle << std::endl;
//             std::cerr << "  "
//                       << "Speed: " << car.speed << std::endl;
//             std::cerr << "  "
//                       << "carId: " << car.carId << std::endl; */
//         }
//     }
// }
