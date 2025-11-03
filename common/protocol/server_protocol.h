#ifndef SERVER_PROTOCOL_H
#define SERVER_PROTOCOL_H

#include "../socket/socket.h"
#include "../messages/snapshot.h"
#include "protocol_consumer.h"

#include <cstdint>
#include <string>

class ServerProtocol {
private:
    Socket& socket;

    // Send methods
    void send_u8(uint8_t value);
    void send_u16(uint16_t value);
    void send_u32(uint32_t value);
    void send_string(const std::string& str);

    // Receive methods
    uint8_t recv_u8();
    uint16_t recv_u16();
    uint32_t recv_u32();
    std::string recv_string();
    
    // Handshake methods
    void send_initial_info();
    std::string recv_username();

public:
    explicit ServerProtocol(Socket& socket);

    // Methods for lobby synchronous communication
    bool consume_one(ProtocolConsumer& consumer);
    void send_created(uint16_t match_id);
    void send_joined(bool success);
    void send_started(bool success);

    // Methods for game communication
    void send_snapshot(const Snapshot& snapshot);
};

#endif // SERVER_PROTOCOL_H
