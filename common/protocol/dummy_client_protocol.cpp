#include "dummy_client_protocol.h"
#include "protocol_constants.h"

#include <cstdint>
#include <arpa/inet.h>
#include <stdexcept>

DummyClientProtocol::DummyClientProtocol(Socket& socket): skt(socket) {}

void DummyClientProtocol::send_move_request(const MoveRequest& request) {
    uint8_t hdr = COD_MOVE;
    uint8_t directions = 0;

    if (request.up) {
        directions |= 0b1000;
    }
    if (request.down) {
        directions |= 0b0100;
    }
    if (request.left) {
        directions |= 0b0010;
    }
    if (request.right) {
        directions |= 0b0001;
    }

    skt.sendall(&hdr, sizeof(hdr));
    skt.sendall(&directions, sizeof(directions));
}

std::vector<std::pair<ClientID, Vector2D>> DummyClientProtocol::recv_positions_response() {
    uint8_t hdr;
    uint8_t n;

    if (skt.recvall(&hdr, sizeof(hdr)) == 0)
        throw std::runtime_error("Connection closed by server");
    
    skt.recvall(&n, sizeof(n));

    std::vector<std::pair<ClientID, Vector2D>> positions;
    for (uint8_t i = 0; i < n; ++i) {
        uint16_t client_id_net;
        int16_t x_net;
        int16_t y_net;

        skt.recvall(&client_id_net, sizeof(client_id_net));
        skt.recvall(&x_net, sizeof(x_net));
        skt.recvall(&y_net, sizeof(y_net));

        ClientID client_id = ntohs(client_id_net);
        int16_t x = ntohs(x_net);
        int16_t y = ntohs(y_net);

        positions.emplace_back(client_id, Vector2D(x, y));
    }

    return positions;
}

DummyClientProtocol::~DummyClientProtocol() {}
