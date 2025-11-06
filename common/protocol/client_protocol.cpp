#include "client_protocol.h"
#include "protocol_constants.h"

#include <arpa/inet.h>
#include <stdexcept>

ClientProtocol::ClientProtocol(Socket& socket) :
    socket(socket) {}

void ClientProtocol::send_u8(uint8_t value) {
    socket.sendAll(&value, sizeof(value));
}

void ClientProtocol::send_u16(uint16_t value) {
    value = htons(value);
    socket.sendAll(&value, sizeof(value));
}

void ClientProtocol::send_u32(uint32_t value) {
    value = htonl(value);
    socket.sendAll(&value, sizeof(value));
}

void ClientProtocol::send_string(const std::string& str) {
    uint16_t length = static_cast<uint16_t>(str.size());
    send_u16(length);
    socket.sendAll(str.data(), length);
}

uint8_t ClientProtocol::recv_u8() {
    uint8_t value;
    socket.recvAll(&value, sizeof(value));
    return value;
}

uint16_t ClientProtocol::recv_u16() {
    uint16_t value;
    socket.recvAll(&value, sizeof(value));
    return ntohs(value);
}

uint32_t ClientProtocol::recv_u32() {
    uint32_t value;
    socket.recvAll(&value, sizeof(value));
    return ntohl(value);
}

std::string ClientProtocol::recv_string() {
    uint16_t length = recv_u16();
    std::string str(length, '\0');
    socket.recvAll(&str[0], length);
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


// GAME

uint8_t ClientProtocol::recv_message_type() {
    return recv_u8();
}

void ClientProtocol::send_move(const MoveRequest& request) {
    send_u8(MSG_MOVE);
    
    // ewmpaquetar direcciones en un byte (bits: up/down/left/right)
    uint8_t directions = 0;
    if (request.up)    directions |= 0b1000;
    if (request.down)  directions |= 0b0100;
    if (request.left)  directions |= 0b0010;
    if (request.right) directions |= 0b0001;
    
    send_u8(directions);
}

void ClientProtocol::send_modifications(bool speed_mod, bool accel_mod) {
    send_u8(MSG_MODIFY_CAR);
    send_u8(speed_mod ? 0x01 : 0x00);
    send_u8(accel_mod ? 0x01 : 0x00);
}

Snapshot ClientProtocol::recv_snapshot() {
    uint16_t countdown = recv_u16();
    uint16_t num_cars = recv_u16();
    
    std::vector<Snapshot::CarSnapshot> cars;
    for (uint16_t i = 0; i < num_cars; ++i) {
        uint16_t id = recv_u16();
        uint32_t x = recv_u32();
        uint32_t y = recv_u32();
        uint16_t angle = recv_u16();
        uint16_t speed = recv_u16();
        uint8_t type = recv_u8();
        
        cars.emplace_back(id, x, y, angle, speed, type);
    }
    
    return Snapshot(countdown, cars);
}

uint8_t ClientProtocol::recv_countdown() {
    return recv_u8();
}

uint8_t ClientProtocol::recv_checkpoint() {
    return recv_u8();
}

CollisionData ClientProtocol::recv_collision() {
    CollisionData collision;
    collision.player_id = recv_u16();
    
    // recibir intensity como uint8_t (0-255) y convertir a float (0.0-1.0)
    uint8_t intensity_byte = recv_u8();
    collision.intensity = intensity_byte / 255.0f;
    
    collision.x = recv_u32();
    collision.y = recv_u32();
    
    return collision;
}

uint16_t ClientProtocol::recv_player_died() {
    return recv_u16();
}

RaceResults ClientProtocol::recv_race_results() {
    RaceResults results;
    
    results.countdown_ms = recv_u32();
    uint16_t num_players = recv_u16();
    
    for (uint16_t i = 0; i < num_players; ++i) {
        RaceResults::PlayerResult player;
        player.player_name = recv_string();
        player.race_time_ms = recv_u32();
        player.total_time_ms = recv_u32();
        
        results.players.push_back(player);
    }
    
    return results;
}

CarProperties ClientProtocol::recv_car_properties() {
    CarProperties props;
    
    props.speed = recv_u16();
    props.acceleration = recv_u16();
    props.health = recv_u16();
    props.mass = recv_u16();
    props.handling = recv_u16();
    props.countdown_ms = recv_u32();
    
    return props;
}

FinalResults ClientProtocol::recv_final_results() {
    FinalResults results;
    
    uint16_t num_standings = recv_u16();
    
    for (uint16_t i = 0; i < num_standings; ++i) {
        FinalResults::FinalStanding standing;
        standing.player_id = recv_u16();
        standing.player_name = recv_string();
        standing.total_time_ms = recv_u32();
        standing.position = recv_u8();
        
        results.standings.push_back(standing);
    }
    
    results.winner_id = recv_u16();
    results.winner_name = recv_string();
    
    return results;
}