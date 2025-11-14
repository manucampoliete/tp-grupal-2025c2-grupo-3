#include "client_protocol.h"

#include <iostream>
#include <stdexcept>
#include <arpa/inet.h>

#include "../../common/protocol/protocolConstants.h"


ClientProtocol::ClientProtocol(Socket& socket): SendProtocol(socket), RecvProtocol(socket) {}


/**
 * HANDSHAKE
 */
ClientID ClientProtocol::recv_client_id() {
    uint8_t action_code = recvU8();
    if (action_code != SEND_CLIENT_ID)
        throw std::runtime_error("Expected initial info from server");
    return recvU16();
}

std::vector<CarInfo> ClientProtocol::recv_initial_info() {
    sendU8(SEND_INITIAL_INFO);
    uint8_t action_code = recvU8();
    if (action_code != SEND_INITIAL_INFO) {
        throw std::runtime_error("Expected initial info from server");
    }

    std::vector<CarInfo> infos;
    uint16_t size = recvU16();
    for (int i = 0; i < size; i++) {
        CarInfo info;
        info.id = recvU8();
        info.name = recvString();
        info.speed = recvU16();
        info.health = recvU16();

        infos.emplace_back(info);
    }
    return infos;
}


/**
 * LOBBY
 */
uint16_t ClientProtocol::send_create(const std::string& username, uint8_t car_id) {
    uint8_t action_code = SEND_CREATE;
    sendU8(action_code);
    sendString(username);
    sendU8(car_id);

    action_code = recvU8();
    uint16_t match_id = recvU16();
    return match_id;
}

bool ClientProtocol::send_join(uint16_t match_id, const std::string& username, uint8_t car_id) {
    uint8_t action_code = SEND_JOIN;
    sendU8(action_code);
    sendU16(match_id);
    sendString(username);
    sendU8(car_id);

    action_code = recvU8();
    return recvU8() == 0x00;  // 0x00 for success, 0x01 for failure
}

void ClientProtocol::send_start() {
    uint8_t action_code = SEND_START;
    sendU8(action_code);  // 0x00 for success, 0x01 for failure
}

void ClientProtocol::recv_start_signal() {
    uint8_t action_code = recvU8();
    if (action_code != SEND_STARTED) {
        throw std::runtime_error("Expected STARTED signal from server");
    }
}


/**
 * GAME
 */
uint8_t ClientProtocol::recv_message_type() { return recvU8(); }

void ClientProtocol::send_move(const ActiveDirections& request) {
    sendU8(SEND_MOVE_STATE);

    // empaquetar direcciones en un byte
    uint8_t directions = 0;
    if (request.up)
        directions |= UP_MASK;
    if (request.down)
        directions |= DOWN_MASK;
    if (request.left)
        directions |= LEFT_MASK;
    if (request.right)
        directions |= RIGHT_MASK;

    sendU8(directions);
}

Snapshot ClientProtocol::recv_snapshot() {
    uint32_t countdown = recvU32();
    uint8_t num_cars = recvU8();

    std::cout << "[PROTOCOL] Snapshot recibido: countdown=" << countdown
              << "ms, num_cars=" << num_cars << std::endl;

    std::vector<Snapshot::CarSnapshot> cars;
    for (uint8_t i = 0; i < num_cars; ++i) {
        uint16_t id = recvU16();
        uint32_t x = recvU32();
        uint32_t y = recvU32();
        uint16_t angle = recvU16();
        uint16_t speed = recvU16();
        uint8_t carId = recvU8();

        std::cout << "  [Car " << i << "] id=" << id << ", pos=(" << x << "," << y << ")"
                  << ", angle=" << angle << ", speed=" << speed << ", carId=" << (int)carId
                  << std::endl;

        cars.emplace_back(id, x, y, angle, speed, carId);
    }

    return Snapshot(countdown, cars);
}

void ClientProtocol::send_modifications(bool speed_mod, bool health_mod) {
    sendU8(MSG_MODIFY_CAR);
    sendU8(speed_mod ? 0x01 : 0x00);
    sendU8(health_mod ? 0x01 : 0x00);
}

uint8_t ClientProtocol::recv_countdown() { return recvU8(); }

uint8_t ClientProtocol::recv_checkpoint() { return recvU8(); }

CollisionData ClientProtocol::recv_collision() {
    CollisionData collision;
    collision.player_id = recvU16();

    // recibir intensity como uint8_t (0-255) y convertir a float (0.0-1.0)
    uint8_t intensity_byte = recvU8();
    collision.intensity = intensity_byte / 255.0f;

    collision.x = recvU32();
    collision.y = recvU32();

    return collision;
}

uint16_t ClientProtocol::recv_player_died() { return recvU16(); }

RaceResults ClientProtocol::recv_race_results() {
    RaceResults results;
    results.countdown_ms = recvU32();
    uint16_t num_players = recvU16();

    for (uint16_t i = 0; i < num_players; ++i) {
        RaceResults::PlayerResult player;
        player.player_name = recvString();
        player.race_time_ms = recvU32();
        player.total_time_ms = recvU32();

        results.players.push_back(player);
    }

    return results;
}

CarProperties ClientProtocol::recv_car_properties() {
    CarProperties props;

    props.speed = recvU16();
    props.acceleration = recvU16();
    props.health = recvU16();
    props.mass = recvU16();
    props.handling = recvU16();
    props.countdown_ms = recvU32();

    return props;
}

FinalResults ClientProtocol::recv_final_results() {
    FinalResults results;
    uint16_t num_standings = recvU16();

    for (uint16_t i = 0; i < num_standings; ++i) {
        FinalResults::FinalStanding standing;
        standing.player_id = recvU16();
        standing.player_name = recvString();
        standing.total_time_ms = recvU32();
        standing.position = recvU8();

        results.standings.push_back(standing);
    }

    results.winner_id = recvU16();
    results.winner_name = recvString();

    return results;
}


/**
 * CHEATS
 */
void ClientProtocol::send_inmortality_request() { sendU8(SEND_INMORTALITY); }

void ClientProtocol::send_insta_win_request() { sendU8(SEND_INSTA_WIN); }

void ClientProtocol::send_insta_lose_request() { sendU8(SEND_INSTA_LOSE); }
