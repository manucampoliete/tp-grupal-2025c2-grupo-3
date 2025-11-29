#include "sendProtocol.h"
#include "../errors/peerDisconnectedError.h"

#include <arpa/inet.h>

void SendProtocol::safeSendAll(const void *data, unsigned int sz) {
    if (skt.sendAll(data, sz) == 0) {
        throw PeerDisconnectedError();
    }
}

void SendProtocol::sendU8(uint8_t value) { safeSendAll(&value, sizeof(value)); }

void SendProtocol::sendU16(uint16_t value) {
    value = htons(value);
    safeSendAll(&value, sizeof(value));
}

void SendProtocol::sendU32(uint32_t value) {
    value = htonl(value);
    safeSendAll(&value, sizeof(value));
}

void SendProtocol::sendString(const std::string& str) {
    uint16_t length = static_cast<uint16_t>(str.size());
    sendU16(length);
    safeSendAll(str.data(), length);
}
