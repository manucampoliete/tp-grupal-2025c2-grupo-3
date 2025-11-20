#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include <cstdint>
#include <string>
#include <vector>

#include "../../common/messages/gameData.h"
#include "../../common/messages/snapshot.h"
#include "../../common/protocol/recvProtocol.h"
#include "../../common/protocol/sendProtocol.h"
#include "../../common/socket/socket.h"
#include "../../common/types/types.h"
#include "../../common/utils/activeDirections.h"
#include "../../common/utils/carinfo.h"


class ClientProtocol: public SendProtocol, public RecvProtocol {
private:
    /**
     * Packs an ActiveDirections object into a byte.
     */
    uint8_t encodeMoveState(const ActiveDirections& activeDirections);

public:
    explicit ClientProtocol(Socket& socket);


    /**
     * HANDSHAKE
     * TODO: (Manu) Unify both methods into a single one?
     */
    ClientID recvClientId();
    std::vector<CarInfo> recvInitialInfo();


    /**
     * LOBBY
     */
    uint16_t sendCreate(const std::string& username, CarID carId);
    bool sendJoin(MatchID matchId, const std::string& username, CarID carId);
    void sendStart();
    void recvStartSignal();


    /**
     * GAME
     */
    uint8_t recvMessageType();

    void sendMove(const ActiveDirections& activeDirections);
    void sendModifications(bool speedMod, bool healthMod);

    Snapshot recvSnapshot();
    uint8_t recvCountdown();
    uint8_t recvCheckpoint();
    CollisionData recvCollision();
    uint16_t recvPlayerDied();
    RaceResults recvRaceResults();
    CarProperties recvCarProperties();
    FinalResults recvFinalResults();

    /**
     * CHEATS
     */
    void sendInmortalityRequest();
    void sendInstaWinRequest();
    void sendInstaLoseRequest();
};

#endif  // CLIENT_PROTOCOL_H
