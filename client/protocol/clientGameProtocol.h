#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include <cstdint>
#include <string>
#include <vector>

#include "../utils/gameData.h"
#include "../../common/messages/snapshot.h"
#include "../../common/protocol/recvProtocol.h"
#include "../../common/protocol/sendProtocol.h"
#include "../../common/socket/socket.h"
#include "../../common/types/types.h"
#include "../../common/utils/activeDirections.h"
#include "../../common/utils/carinfo.h"


class ClientGameProtocol: public SendProtocol, public RecvProtocol {
private:
    /**
     * Packs an ActiveDirections object into a byte.
     */
    uint8_t encodeMoveState(const ActiveDirections& activeDirections);

public:
    explicit ClientGameProtocol(Socket& socket);
    
    uint8_t recvMessageType();

    void sendMove(const ActiveDirections& activeDirections);
    void sendModifications(bool speedMod, bool healthMod, bool accelMod, bool massMod);

    Snapshot recvSnapshot();
    uint8_t recvCountdown();
    uint8_t recvCheckpoint();
    CollisionData recvCollision();
    uint16_t recvPlayerDied();
    RaceResults recvRaceResults();
    uint8_t recvStatsCountdown();
    std::vector<CarProperties> recvCarProperties();
    uint8_t recvModCountdown();
    FinalResults recvFinalResults();

    /**
     * CHEATS
     */
    void sendInmortalityRequest();
    void sendInstaWinRequest();
    void sendInstaLoseRequest();
    void sendSuperSpeedRequest();
};

#endif  // CLIENT_PROTOCOL_H
