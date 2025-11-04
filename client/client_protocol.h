#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include "../common/socket/socket.h"

#include <cstdint>
#include <string>

class ClientProtocol {
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
    void recv_initial_info();

public:
    explicit ClientProtocol(Socket& socket);

    uint16_t send_create(const std::string& username, uint8_t car_id);

    bool send_join(uint16_t match_id, const std::string& username, uint8_t car_id);

    bool send_start();

    void recv_start_signal();
};
#endif // CLIENT_PROTOCOL_H
