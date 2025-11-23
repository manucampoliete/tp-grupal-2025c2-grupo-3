#include "clientHandshakeProtocol.h"
#include "../../common/protocol/protocolConstants.h"

#include <stdexcept>

ClientID ClientHandshakeProtocol::recvClientId() {
    if (recvU8() != SEND_CLIENT_ID)
        throw std::runtime_error("Expected SEND_CLIENT_ID response from server");
    return recvU16();  // client ID
}
