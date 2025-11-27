#ifndef SEND_PROTOCOL_H
#define SEND_PROTOCOL_H

#include <cstdint>
#include <string>

#include "../socket/socket.h"

/**
 * Class that provides methods to send various data types over a socket.
 */
class SendProtocol {
private:
    Socket& skt;

    void safeSendAll(const void *data, unsigned int sz);

public:
    /**
     * Constructor that takes a reference to a Socket object.
     */
    explicit SendProtocol(Socket& skt): skt(skt) {}

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
