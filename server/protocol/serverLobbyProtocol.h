#ifndef SERVER_LOBBY_PROTOCOL_H
#define SERVER_LOBBY_PROTOCOL_H

#include "../../common/protocol/recvProtocol.h"
#include "../../common/protocol/sendProtocol.h"
#include "../../common/socket/socket.h"
#include "../requestsResolving/lobbyResolver.h"

class LobbyResolver;

class ServerLobbyProtocol: public RecvProtocol, public SendProtocol {
private:
    void sendInitialInfo();

    /**
     * Receives a SEND_CREATE message and processes it using the given LobbyResolver.
     */
    void recvCreateMatch(LobbyResolver& lobbyResolver);

    /**
     * Receives a SEND_JOIN message and processes it using the given LobbyResolver.
     */
    void recvJoinMatch(LobbyResolver& lobbyResolver);

    /**
     * Receives a SEND_START message and processes it using the given LobbyResolver.
     */
    void recvStartMatch(LobbyResolver& lobbyResolver);

public:
    /**
     * Constructor that takes a reference to a Socket object.
     */
    ServerLobbyProtocol(Socket& skt, ClientID clientId);

    /**
     * Consumes one message from the socket and processes it using the given LobbyResolver.
     */
    void consumeOne(LobbyResolver& lobbyResolver);

    /**
     * Sends a SEND_CREATED message with the given matchId.
     */
    void sendCreated(MatchID matchId);

    /**
     * Sends a SEND_JOINED message with the given success status.
     */
    void sendJoined(bool success);

    /**
     * Sends a SEND_STARTED message with the given success status.
     */
    void sendStarted(bool success);
};

#endif  // SERVER_LOBBY_PROTOCOL_H
