#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include <cstdint>
#include <string>

#include "../../common/protocol/sendProtocol.h"
#include "../../common/protocol/recvProtocol.h"
#include "../utils/activeDirections.h"
#include "../types/types.h"

class ClientProtocol : public SendProtocol, public RecvProtocol {
private:
    /**
     * Handshake method to receive initial information from the server.
     */
    void recvInitialInfo();

    /**
     * Encodes ActiveDirections into a single byte.
     */
    uint8_t encodeActiveDirections(const ActiveDirections& activeDirections);

public:
    /**
     * Constructor that takes a reference to a Socket object.
     */
    explicit ClientProtocol(Socket& skt);

    /**
     * Sends a create match request to the server.
     */
    MatchID sendCreate(const std::string& username, CarID carId);

    /**
     * Sends a join match request to the server.
     */
    bool sendJoin(MatchID matchId, const std::string& username, CarID carId);

    /**
     * Sends a start match request to the server.
     */
    bool sendStart(MatchID matchId);

    /**
     * Sends the player's move to the server.
     */
    void sendMove(const ActiveDirections& activeDirections);

    /**
     * Receives the start signal from the server.
     */
    void recvStartSignal();
};
#endif  // CLIENT_PROTOCOL_H
