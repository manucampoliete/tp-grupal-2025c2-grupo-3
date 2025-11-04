
#include "dummy_server_protocol.h"
#include "protocol_constants.h"

#include <cstdint>
#include <arpa/inet.h>
#include <stdexcept>

DummyServerProtocol::DummyServerProtocol(Socket& socket): skt(socket) {}

MoveRequest DummyServerProtocol::recv_move_request() {
    uint8_t hdr;
    uint8_t directions;
    bool up, down, left, right;

    if (skt.recvall(&hdr, sizeof(hdr)) == 0)
        throw std::runtime_error("Connection closed by peer");
    
    skt.recvall(&directions, sizeof(directions));

    up = directions & 0b1000;
    down = directions & 0b0100;
    left = directions & 0b0010;
    right = directions & 0b0001;

    return MoveRequest(up, down, left, right);
}

void DummyServerProtocol::send_positions_response(const std::vector<std::pair<ClientID, Vector2D>>& positions) {
    uint8_t hdr = COD_POSITIONS;
    uint8_t count = static_cast<uint8_t>(positions.size());

    skt.sendall(&hdr, sizeof(hdr));
    skt.sendall(&count, sizeof(count));

    for (const auto& pair : positions) {
        ClientID client_id = pair.first;
        Vector2D position = pair.second;

        uint16_t client_id_net = htons(client_id);
        int16_t x_net = htons(position.x);
        int16_t y_net = htons(position.y);

        skt.sendall(&client_id_net, sizeof(client_id_net));
        skt.sendall(&x_net, sizeof(x_net));
        skt.sendall(&y_net, sizeof(y_net));
    }
}

DummyServerProtocol::~DummyServerProtocol() {}
