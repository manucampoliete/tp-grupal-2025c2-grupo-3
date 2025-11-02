#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <cstdint>
#include <string>
#include <arpa/inet.h> // For htons
#include "../socket/socket.h"

class Serializer {
private:
    Socket &skt;

public:
    Serializer(Socket &socket) : skt(socket) {}

    void send_byte(uint8_t value) {
        skt.sendall(&value, sizeof(value));
    }

    void send_short(uint16_t value) {
        uint16_t value_net = htons(value);
        skt.sendall(&value_net, sizeof(value_net));
    }

    void send_int(uint32_t value) {
        uint32_t value_net = htonl(value);
        skt.sendall(&value_net, sizeof(value_net));
    }

    void send_string(const std::string &str) {
        uint16_t length = static_cast<uint16_t>(str.size());
        uint16_t length_net = htons(length);
        skt.sendall(&length_net, sizeof(length_net));
        skt.sendall(str.data(), length);
    }

    ~Serializer() {}
};

#endif // SERIALIZER_H
