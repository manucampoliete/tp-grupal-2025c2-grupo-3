#ifndef CLIENT_GAME_SEND_PROTOCOL_H
#define CLIENT_GAME_SEND_PROTOCOL_H

#include "../../common/protocol/sendProtocol.h"
#include "../../common/utils/activeDirections.h"

class ClientGameSendProtocol: public SendProtocol {
private:
    /**
     * Packs an ActiveDirections object into a byte.
     */
    uint8_t encodeMoveState(const ActiveDirections& activeDirections);

public:
    /**
     * Constructor that takes a reference to a Socket object.
     */
    explicit ClientGameSendProtocol(Socket& skt);

    /**
     * Sends the player's move directions to the server.
     */
    void sendMove(const ActiveDirections& activeDirections);

    /**
     * Sends car modification requests to the server.
     */
    void sendModifications(bool speedMod, bool healthMod, bool accelMod, bool massMod);

    /**
     * CHEATS
     */
    void sendInmortalityRequest();
    void sendInstaWinRequest();
    void sendInstaLoseRequest();
    void sendSuperSpeedRequest();
};

#endif  // CLIENT_GAME_SEND_PROTOCOL_H
