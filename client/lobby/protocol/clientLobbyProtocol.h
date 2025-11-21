#ifndef CLIENT_LOBBY_PROTOCOL_H
#define CLIENT_LOBBY_PROTOCOL_H

#include "../../../common/protocol/recvProtocol.h"
#include "../../../common/protocol/sendProtocol.h"
#include "../../../common/types/types.h"
#include "../../../common/utils/carinfo.h"

#include <vector>

class ClientLobbyProtocol : public SendProtocol, public RecvProtocol {
public:
    /**
     * Constructor that takes a reference to a Socket object.
     */
    ClientLobbyProtocol(Socket& skt);
    
    /**
     * Receives the initial car information from the server (id, name, speed, health).
     */
    std::vector<CarInfo> recvInitialInfo();

    /**
     * Sends a create match request to the server with the given username and car ID.
     * Returns the match ID assigned by the server.
     */
    uint16_t sendCreate(const std::string& username, CarID carId);

    /**
     * Sends a join match request to the server with the given match ID, username, and car ID.
     * Returns true if the join was successful, false otherwise.
     */
    bool sendJoin(MatchID matchId, const std::string& username, CarID carId);

    /**
     * Sends a start game request to the server.
     */
    void sendStart();

    /**
     * Receives a start signal from the server.
     */
    void recvStartSignal();
};

#endif // CLIENT_LOBBY_PROTOCOL_H
