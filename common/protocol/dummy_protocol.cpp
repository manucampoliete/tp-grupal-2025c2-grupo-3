
#include "dummy_protocol.h"

#include <cstdint>
#include <vector>

#include <arpa/inet.h>

DummyProtocol::DummyProtocol(Socket& socket): skt(socket) {}

std::string DummyProtocol::recv_message() {
    uint8_t hdr;
    uint16_t len;
    skt.recvall(&hdr, sizeof(hdr));
    skt.recvall(&len, sizeof(len));
    len = ntohs(len);
    std::string s(len, '\0');
    skt.recvall(s.data(), len);
    return s;
}

void DummyProtocol::send_message(const std::string& message) {
    uint8_t hdr = 0xFF;
    uint16_t len = htons(static_cast<uint16_t>(message.size()));
    skt.sendall(&hdr, sizeof(hdr));
    skt.sendall(&len, sizeof(len));
    skt.sendall(message.data(), message.size());
}

DummyProtocol::~DummyProtocol() {}
