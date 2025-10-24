
#ifndef DUMMY_PROTOCOL_H
#define DUMMY_PROTOCOL_H

#include <string>

#include "../socket/socket.h"

class DummyProtocol {
private:
    Socket& skt;

public:
    /**
     * Constructor: takes a Socket reference to use for communication
     */
    explicit DummyProtocol(Socket& socket);

    /**
     * Receives a message from the socket following the dummy protocol.
     * Returns the received message as a string.
     */
    std::string recv_message();

    /**
     * Sends a message through the socket following the dummy protocol.
     */
    void send_message(const std::string& message);

    /**
     * Destructor
     * Nothing special to do
     */
    ~DummyProtocol();
};

#endif  // DUMMY_PROTOCOL_H
