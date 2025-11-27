#ifndef RECV_PROTOCOL_H
#define RECV_PROTOCOL_H

#include <cstdint>
#include <string>

#include "../socket/socket.h"

/**
 * Class that provides methods to receive various data types over a socket.
 */
class RecvProtocol {
private:
    Socket& skt;

    void safeRecvAll(void *data, unsigned int sz);

public:
    /**
     * Constructor that takes a reference to a Socket object.
     */
    explicit RecvProtocol(Socket& skt): skt(skt) {}

    /**
     * Receives an 8-bit unsigned integer from the socket.
     */
    uint8_t recvU8();

    /**
     * Receives a 16-bit unsigned integer from the socket.
     */
    uint16_t recvU16();

    /**
     * Receives a 32-bit unsigned integer from the socket.
     */
    uint32_t recvU32();

    /**
     * Receives a string from the socket.
     */
    std::string recvString();
};

#endif  // RECV_PROTOCOL_H
