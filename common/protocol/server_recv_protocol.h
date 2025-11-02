#ifndef SERVER_RECV_PROTOCOL_H
#define SERVER_RECV_PROTOCOL_H

#include "protocol_consumer.h"
#include "../socket/socket.h"

#include <cstdint>
#include <string>

class ServerRecvProtocol {
private:
    bool running;
    Socket& socket;

    uint8_t recv_u8();
    uint16_t recv_u16();
    uint32_t recv_u32();
    std::string recv_string();

    void send_initial_info();
    std::string recv_username();

public:
    explicit ServerRecvProtocol(Socket& socket);

    void consumeOne(ProtocolConsumer& consumer);
    void startConsuming(ProtocolConsumer& consumer); // Infinite loop
};

#endif // SERVER_RECV_PROTOCOL_H
