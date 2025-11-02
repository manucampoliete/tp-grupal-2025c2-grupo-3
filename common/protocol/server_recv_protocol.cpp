#include "server_recv_protocol.h"
#include "protocol_constants.h"

#include <arpa/inet.h> // For ntohs
#include <stdexcept>

uint8_t ServerRecvProtocol::recv_u8() {
    uint8_t value;
    socket.recvall(&value, sizeof(value));
    return value;
}

uint16_t ServerRecvProtocol::recv_u16() {
    uint16_t value;
    socket.recvall(&value, sizeof(value));
    return ntohs(value);
}

uint32_t ServerRecvProtocol::recv_u32() {
    uint32_t value;
    socket.recvall(&value, sizeof(value));
    return ntohl(value);
}

std::string ServerRecvProtocol::recv_string() {
    uint16_t length = recv_u16();
    std::string str(length, '\0');
    socket.recvall(&str[0], length);
    return str;
}

void ServerRecvProtocol::send_initial_info() {
    uint8_t action_code = SEND_INITIAL_INFO;
    socket.sendall(&action_code, sizeof(action_code));

    /**
     * TODO: send actual initial info
     */
}

std::string ServerRecvProtocol::recv_username() {
    uint8_t action_code = recv_u8();
    if (action_code != SEND_USERNAME) {
        throw std::runtime_error("Expected SEND_USERNAME action code");
    }
    return recv_string();
}

ServerRecvProtocol::ServerRecvProtocol(Socket& socket) : socket(socket), running(true) {
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

void ServerRecvProtocol::consumeOne(ProtocolConsumer& consumer) {
    uint8_t action_code = recv_u8();

    switch (action_code) {
        case SEND_CREATE: {
            uint8_t car_id = recv_u8();
            consumer.on_create_match(car_id);
            break;
        }
        case SEND_JOIN: {
            uint16_t match_id = recv_u16();
            uint8_t car_id = recv_u8();
            consumer.on_join_match(match_id, car_id);
            break;
        }
        case SEND_START: {
            consumer.on_start_match();
            break;
        }
        case SEND_MOVE_STATE: {
            uint8_t directions = recv_u8();
            consumer.on_move(directions);
            break;
        }
        default: {
            throw std::runtime_error("Unknown action code");
        }
    }
}

void ServerRecvProtocol::startConsuming(ProtocolConsumer& consumer) {
    while (running) {
        consumeOne(consumer);
    }
}