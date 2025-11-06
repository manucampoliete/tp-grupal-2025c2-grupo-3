#include "sendProtocol.h"

#include <arpa/inet.h>

void SendProtocol::sendU8(uint8_t value) { skt.sendAll(&value, sizeof(value)); }

void SendProtocol::sendU16(uint16_t value) {
    value = htons(value);
    skt.sendAll(&value, sizeof(value));
}

void SendProtocol::sendU32(uint32_t value) {
    value = htonl(value);
    skt.sendAll(&value, sizeof(value));
}

void SendProtocol::sendString(const std::string& str) {
    uint16_t length = static_cast<uint16_t>(str.size());
    sendU16(length);
    skt.sendAll(str.data(), length);
}
