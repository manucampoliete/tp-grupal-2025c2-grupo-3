#ifndef SERVER_SEND_PROTOCOL_H
#define SERVER_SEND_PROTOCOL_H

#include "../messages/snapshot.h"
#include "../socket/socket.h"

#include <cstdint>
#include <string>

class ServerSendProtocol {
private:
    Socket& socket;

    void send_u8(uint8_t value);
    void send_u16(uint16_t value);
    void send_u32(uint32_t value);
    void send_string(const std::string& str);

public:
    explicit ServerSendProtocol(Socket& socket);
    
    void send_snapshot(const Snapshot& snapshot);
};

#endif // SERVER_SEND_PROTOCOL_H
