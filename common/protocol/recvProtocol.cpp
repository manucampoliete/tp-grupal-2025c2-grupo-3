#include "recvProtocol.h"

#include <arpa/inet.h>
#include <stdexcept>

void RecvProtocol::safeRecvAll(void *data, unsigned int sz) {
    if (skt.recvAll(data, sz) == 0) {
        throw std::runtime_error("Connection closed while receiving data");
    }
}

uint8_t RecvProtocol::recvU8() {
    uint8_t value;
    safeRecvAll(&value, sizeof(value));
    return value;
}

uint16_t RecvProtocol::recvU16() {
    uint16_t value;
    safeRecvAll(&value, sizeof(value));
    return ntohs(value);
}

uint32_t RecvProtocol::recvU32() {
    uint32_t value;
    safeRecvAll(&value, sizeof(value));
    return ntohl(value);
}

std::string RecvProtocol::recvString() {
    uint16_t length = recvU16();
    std::string str(length, '\0');
    safeRecvAll(&str[0], length);
    return str;
}
