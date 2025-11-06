#ifndef SEND_PROTOCOL_H
#define SEND_PROTOCOL_H

#include "../socket/socket.h"

#include <cstdint>
#include <string>

/**
 * Class that provides methods to send various data types over a socket.
 */
class SendProtocol {
private:
    Socket& skt;

public:
    /**
     * Constructor that takes a reference to a Socket object.
     */
    explicit SendProtocol(Socket& skt) : skt(skt) {}

    /**
     * Sends an 8-bit unsigned integer over the socket.
     */
    void sendU8(uint8_t value);

    /**
     * Sends a 16-bit unsigned integer over the socket.
     */
    void sendU16(uint16_t value);

    /**
     * Sends a 32-bit unsigned integer over the socket.
     */
    void sendU32(uint32_t value);

    /**
     * Sends a string over the socket.
     */
    void sendString(const std::string& str);
};

#endif  // SEND_PROTOCOL_H
