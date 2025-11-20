#ifndef CLIENT_HANDSHAKE_PROTOCOL_H
#define CLIENT_HANDSHAKE_PROTOCOL_H

#include "../../common/protocol/recvProtocol.h"
#include "../../common/types/types.h"

class ClientHandshakeProtocol : public RecvProtocol {
public:
    ClientHandshakeProtocol(Socket& socket)
        : RecvProtocol(socket) {}

    ClientID recvClientId();
};

#endif // CLIENT_HANDSHAKE_PROTOCOL_H
