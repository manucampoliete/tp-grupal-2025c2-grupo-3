#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include <cstdint>
#include <string>
#include <arpa/inet.h> // For ntohs, ntohl
#include "../socket/socket.h"

class Deserializer {
private:
    Socket &skt;

public:
    Deserializer(Socket &socket) : skt(socket) {}

    uint8_t recv_byte() {
        uint8_t value;
        skt.recvAll(&value, sizeof(value));
        return value;
    }

    uint16_t recv_short() {
        uint16_t value;
        skt.recvAll(&value, sizeof(value));
        return ntohs(value);
    }

    uint32_t recv_int() {
        uint32_t value;
        skt.recvAll(&value, sizeof(value));
        return ntohl(value);
    }

    std::string recv_string() {
        uint16_t length_net;
        skt.recvAll(&length_net, sizeof(length_net));
        uint16_t length = ntohs(length_net);

        std::string str(length, '\0');
        skt.recvAll(&str[0], length);
        return str;
    }

    ~Deserializer() {}
};

#endif // DESERIALIZER_H
