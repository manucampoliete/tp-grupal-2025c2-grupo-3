#ifndef SERVER_GAME_SEND_PROTOCOL_H
#define SERVER_GAME_SEND_PROTOCOL_H

#include <memory>

#include "../../common/messages/snapshot.h"
#include "../../common/protocol/sendProtocol.h"

class ServerGameSendProtocol: public SendProtocol {
public:
    /**
     * Constructor that takes a reference to a Socket object.
     */
    explicit ServerGameSendProtocol(Socket& socket);

    /**
     * Sends a Snapshot message over the socket.
     */
    void sendSnapshot(std::shared_ptr<Snapshot> snapshot);

    void sendRaceSnapshot(std::shared_ptr<Snapshot> snapshot);

    void sendRaceResultsSnapshot(std::shared_ptr<Snapshot> snapshot);

    void sendModificationSnapshot(std::shared_ptr<Snapshot> snapshot);
    void sendCollisionSnapshot(const Snapshot::CollisionData& collision);
};

#endif  // SERVER_GAME_SEND_PROTOCOL_H
