#ifndef LOBBY_RESOLVER_H
#define LOBBY_RESOLVER_H

#include <memory>
#include <string>

#include "../../common/messages/snapshot.h"
#include "../../common/queue/queue.h"
#include "../../common/types/types.h"
#include "../commands/command.h"
#include "../protocol/serverLobbyProtocol.h"
#include "../synchronized/matchesMapMonitor.h"

class ServerLobbyProtocol;

class LobbyResolver {
private:
    ClientID clientId;
    MatchesMapMonitor& matchesMapMonitor;
    bool inLobbyPhase;
    MatchID matchIdCopy;

public:
    /**
     * Constructor
     */
    LobbyResolver(ClientID clientId, MatchesMapMonitor& matchesMapMonitor);

    /**
     * Handles a CREATE_MATCH request from the client.
     */
    void handleCreateMatch(const std::string& username, uint8_t carId,
                           ServerLobbyProtocol& protocol);

    /**
     * Handles a JOIN_MATCH request from the client.
     */
    void handleJoinMatch(MatchID matchId, const std::string& username, uint8_t carId,
                         ServerLobbyProtocol& protocol);

    /**
     * Handles a START_MATCH request from the client.
     */
    void handleStartMatch(ServerLobbyProtocol& protocol);

    /**
     * Returns true if the client is in the lobby phase, false otherwise.
     */
    bool isInLobbyPhase() const;

    /**
     * Returns a reference to the clientCommands queue for the current client.
     */
    Queue<std::unique_ptr<Command>>& getClientCommandsQueue();

    /**
     * Returns a reference to the responses queue for the current client.
     */
    Queue<std::shared_ptr<Snapshot>>& getResponsesQueue();
};

#endif  // LOBBY_RESOLVER_H
