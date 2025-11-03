#include "client_protocol.h"
#include "protocol_constants.h"

#include <arpa/inet.h>
#include <stdexcept>

void ClientProtocol::send_u8(uint8_t value) {
    socket.sendall(&value, sizeof(value));
}

void ClientProtocol::send_u16(uint16_t value) {
    value = htons(value);
    socket.sendall(&value, sizeof(value));
}

void ClientProtocol::send_u32(uint32_t value) {
    value = htonl(value);
    socket.sendall(&value, sizeof(value));
}

void ClientProtocol::send_string(const std::string& str) {
    uint16_t length = static_cast<uint16_t>(str.size());
    send_u16(length);
    socket.sendall(str.data(), length);
}

uint8_t ClientProtocol::recv_u8() {
    uint8_t value;
    socket.recvall(&value, sizeof(value));
    return value;
}

uint16_t ClientProtocol::recv_u16() {
    uint16_t value;
    socket.recvall(&value, sizeof(value));
    return ntohs(value);
}

uint32_t ClientProtocol::recv_u32() {
    uint32_t value;
    socket.recvall(&value, sizeof(value));
    return ntohl(value);
}

std::string ClientProtocol::recv_string() {
    uint16_t length = recv_u16();
    std::string str(length, '\0');
    socket.recvall(&str[0], length);
    return str;
}

void ClientProtocol::recv_initial_info() {
    uint8_t action_code = recv_u8();
    if (action_code != SEND_INITIAL_INFO) {
        throw std::runtime_error("Expected initial info from server");
    }
    
    // Deserializar + construir struct
}

uint16_t ClientProtocol::send_create(const std::string& username, uint8_t car_id) {
    uint8_t action_code = SEND_CREATE;
    send_u8(action_code);
    send_string(username);
    send_u8(car_id);

    action_code = recv_u8();
    uint16_t match_id = recv_u16();
    return match_id;
}

bool ClientProtocol::send_join(uint16_t match_id, const std::string& username, uint8_t car_id) {
    uint8_t action_code = SEND_JOIN;
    send_u8(action_code);
    send_u16(match_id);
    send_string(username);
    send_u8(car_id);

    action_code = recv_u8();
    return recv_u8() == 0x00;  // 0x00 for success, 0x01 for failure
}

bool ClientProtocol::send_start() {
    uint8_t action_code = SEND_START;
    send_u8(action_code);

    action_code = recv_u8();
    return recv_u8() == 0x00;  // 0x00 for success, 0x01 for failure
}

void ClientProtocol::recv_start_signal() {
    uint8_t action_code = recv_u8();
    if (action_code != SEND_STARTED) {
        throw std::runtime_error("Expected STARTED signal from server");
    }
}
