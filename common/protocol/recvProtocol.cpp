#include "recvProtocol.h"

#include <arpa/inet.h>

uint8_t RecvProtocol::recvU8() {
    uint8_t value;
    skt.recvAll(&value, sizeof(value));
    return value;
}

uint16_t RecvProtocol::recvU16() {
    uint16_t value;
    skt.recvAll(&value, sizeof(value));
    return ntohs(value);
}

uint32_t RecvProtocol::recvU32() {
    uint32_t value;
    skt.recvAll(&value, sizeof(value));
    return ntohl(value);
}

std::string RecvProtocol::recvString() {
    uint16_t length = recvU16();
    std::string str(length, '\0');
    skt.recvAll(&str[0], length);
    return str;
}
