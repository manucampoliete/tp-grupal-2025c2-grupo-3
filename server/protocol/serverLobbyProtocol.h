#ifndef SERVER_LOBBY_PROTOCOL_H
#define SERVER_LOBBY_PROTOCOL_H

#include "../../common/protocol/recvProtocol.h"
#include "../../common/protocol/sendProtocol.h"
#include "../../common/socket/socket.h"
#include "../requestsResolving/lobbyResolver.h"

class LobbyResolver;

class ServerLobbyProtocol: public RecvProtocol, public SendProtocol {
private:
    LobbyResolver& lobbyResolver;

    /**
     * Sends a SEND_CLIENT_ID message with the given clientId.
     */
    void sendClientID(ClientID clientId);

    /**
     * Sends the initial information to the client.
     */
    void sendInitialInfo();

    /**
     * Receives a SEND_CREATE message and processes it using the given LobbyResolver.
     */
    void recvCreateMatch();

    /**
     * Receives a SEND_JOIN message and processes it using the given LobbyResolver.
     */
    void recvJoinMatch();

    /**
     * Receives a SEND_START message and processes it using the given LobbyResolver.
     */
    void recvStartMatch();

public:
    /**
     * Constructor that takes a reference to a Socket object.
     */
    ServerLobbyProtocol(Socket& skt, LobbyResolver& lobbyResolver, ClientID clientId);

    /**
     * Consumes one message from the socket and processes it using the given LobbyResolver.
     */
    void consumeOne();

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
    /** void sendStarted(bool success); */
};

#endif  // SERVER_LOBBY_PROTOCOL_H
