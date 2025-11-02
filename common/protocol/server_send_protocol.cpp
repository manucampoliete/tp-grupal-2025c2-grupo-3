#include "server_send_protocol.h"

#include <arpa/inet.h>

void ServerSendProtocol::send_u8(uint8_t value) {
    socket.sendall(&value, sizeof(value));
}

void ServerSendProtocol::send_u16(uint16_t value) {
    value = htons(value);
    socket.sendall(&value, sizeof(value));
}

void ServerSendProtocol::send_u32(uint32_t value) {
    value = htonl(value);
    socket.sendall(&value, sizeof(value));
}

void ServerSendProtocol::send_string(const std::string& str) {
    uint16_t length = static_cast<uint16_t>(str.size());
    send_u16(length);
    socket.sendall(str.data(), length);
}

ServerSendProtocol::ServerSendProtocol(Socket& socket) : socket(socket) {}

void ServerSendProtocol::send_snapshot(const Snapshot& snapshot) {
    // Send countdown
    send_u16(snapshot.countdown);

    // Send number of cars
    uint16_t num_cars = static_cast<uint16_t>(snapshot.cars.size());
    send_u16(num_cars);

    // Send each car's snapshot
    for (const auto& car : snapshot.cars) {
        send_u16(car.id);
        send_u16(car.x);
        send_u16(car.y);
        send_u16(car.angle);
        send_u16(car.speed);
        send_u8(car.type);
    }
}
