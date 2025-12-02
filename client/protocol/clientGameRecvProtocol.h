#ifndef CLIENT_GAME_RECV_PROTOCOL_H
#define CLIENT_GAME_RECV_PROTOCOL_H

#include "../../common/protocol/recvProtocol.h"
#include "../../common/messages/snapshot.h"
#include "../utils/gameData.h"

class ClientGameRecvProtocol: public RecvProtocol {
public:
    /**
     * Constructor that takes a reference to a Socket object.
     */
    explicit ClientGameRecvProtocol(Socket& skt);
    
    /**
     * Receives the message type from the server.
     */
    uint8_t recvMessageType();

    /**
     * All message receiving methods
     */
    Snapshot recvSnapshot();
    RaceInfo recvRaceInfo();
    uint8_t recvCountdown();
    CollisionData recvCollision();
    uint16_t recvPlayerDied();
    RaceResults recvRaceResults();
    uint8_t recvStatsCountdown();
    std::vector<CarProperties> recvCarProperties();
    uint8_t recvModCountdown();
    FinalResults recvFinalResults();
};

#endif  // CLIENT_GAME_RECV_PROTOCOL_H
