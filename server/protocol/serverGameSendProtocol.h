#ifndef SERVER_GAME_SEND_PROTOCOL_H
#define SERVER_GAME_SEND_PROTOCOL_H

#include "sendProtocol.h"
#include "../messages/snapshot.h"

#include <memory>

class ServerGameSendProtocol : public SendProtocol {
public:
    /**
     * Constructor that takes a reference to a Socket object.
     */
    explicit ServerGameSendProtocol(Socket& socket);
    
    /**
     * Sends a Snapshot message over the socket.
     */
    void sendSnapshot(std::shared_ptr<Snapshot> snapshot);
};

#endif // SERVER_GAME_SEND_PROTOCOL_H
