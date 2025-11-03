#include "server_protocol.h"
#include "protocol_constants.h"

#include <arpa/inet.h>
#include <stdexcept>

// Send methods
void ServerProtocol::send_u8(uint8_t value) {
    socket.sendall(&value, sizeof(value));
}

void ServerProtocol::send_u16(uint16_t value) {
    value = htons(value);
    socket.sendall(&value, sizeof(value));
}

void ServerProtocol::send_u32(uint32_t value) {
    value = htonl(value);
    socket.sendall(&value, sizeof(value));
}

void ServerProtocol::send_string(const std::string& str) {
    uint16_t length = static_cast<uint16_t>(str.size());
    send_u16(length);
    socket.sendall(str.data(), length);
}

// Receive methods
uint8_t ServerProtocol::recv_u8() {
    uint8_t value;
    socket.recvall(&value, sizeof(value));
    return value;
}

uint16_t ServerProtocol::recv_u16() {
    uint16_t value;
    socket.recvall(&value, sizeof(value));
    return ntohs(value);
}

uint32_t ServerProtocol::recv_u32() {
    uint32_t value;
    socket.recvall(&value, sizeof(value));
    return ntohl(value);
}

std::string ServerProtocol::recv_string() {
    uint16_t length = recv_u16();
    std::string str(length, '\0');
    socket.recvall(&str[0], length);
    return str;
}

// Handshake methods
void ServerProtocol::send_initial_info() {
    uint8_t action_code = SEND_INITIAL_INFO;
    socket.sendall(&action_code, sizeof(action_code));

    /**
     * TODO: send actual initial info
     */
}

std::string ServerProtocol::recv_username() {
    uint8_t action_code = recv_u8();
    if (action_code != SEND_USERNAME) {
        throw std::runtime_error("Expected SEND_USERNAME action code");
    }
    return recv_string();
}

ServerProtocol::ServerProtocol(Socket& socket) : socket(socket) {
    /**
     * This is the handshake
     */

    // Send initial information
    send_initial_info();
    // Receive username
    std::string username = recv_username();

    /**
     * TODO: Do something with the username (e.g., store it)
     */
}

// Methods for lobby synchronous communication
bool ServerProtocol::consume_one(ProtocolConsumer& consumer) {
    uint8_t action_code = recv_u8();

    switch (action_code) {
        case SEND_CREATE: {
            std::string username = recv_string();
            uint8_t car_id = recv_u8();
            consumer.on_create_match(username, car_id);
            return false;
            break;
        }
        case SEND_JOIN: {
            uint16_t match_id = recv_u16();
            std::string username = recv_string();
            uint8_t car_id = recv_u8();
            consumer.on_join_match(match_id, username, car_id);
            return false;
            break;
        }
        case SEND_START: {
            consumer.on_start_match();
            return true;
            break;
        }
        case SEND_MOVE_STATE: {
            uint8_t directions = recv_u8();
            ActiveDirections active_directions(
                directions & 0b1000,
                directions & 0b0100,
                directions & 0b0010,
                directions & 0b0001
            );
            consumer.on_move(active_directions);
            return false;
            break;
        }
        default: {
            throw std::runtime_error("Unknown action code");
        }
    }
}

void ServerProtocol::send_created(MatchID match_id) {
    uint8_t action_code = SEND_CREATED;
    send_u8(action_code);
    send_u16(match_id);
}

void ServerProtocol::send_joined(bool success) {
    uint8_t action_code = SEND_JOINED;
    send_u8(action_code);
    send_u8(success ? 0x00 : 0x01);
}

void ServerProtocol::send_started(bool success) {
    uint8_t action_code = SEND_STARTED;
    send_u8(action_code);
    send_u8(success ? 0x00 : 0x01);
}

// Methods for game communication
void ServerProtocol::send_snapshot(const Snapshot& snapshot) {
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