#include "lobbyResolver.h"

LobbyResolver::LobbyResolver(ClientID clientId, MatchesMapMonitor& matchesMapMonitor):
        clientId(clientId),
        matchesMapMonitor(matchesMapMonitor),
        inLobbyPhase(true),
        matchIdCopy(UINT16_MAX) {}

void LobbyResolver::handleCreateMatch(const std::string& username, uint8_t carId,
                                      ServerLobbyProtocol& protocol) {
    matchIdCopy = matchesMapMonitor.createMatch(clientId, username, carId);
    protocol.sendCreated(matchIdCopy);
    /**
     * TODO:
     * - Handle error case (match creation failed)
     */
}

void LobbyResolver::handleJoinMatch(MatchID matchId, const std::string& username, uint8_t carId,
                                    ServerLobbyProtocol& protocol) {
    matchIdCopy = matchId;
    bool joined = matchesMapMonitor.joinMatch(clientId, matchId, username, carId);
    protocol.sendJoined(joined);
    inLobbyPhase = !joined;
}

void LobbyResolver::handleStartMatch(MatchID matchId, ServerLobbyProtocol& protocol) {
    bool started = matchesMapMonitor.startMatch(matchId);
    protocol.sendStarted(started);
    inLobbyPhase = !started;
}

bool LobbyResolver::isInLobbyPhase() const { return inLobbyPhase; }

Queue<std::unique_ptr<Command>>& LobbyResolver::getClientCommandsQueue() {
    return matchesMapMonitor.getClientCommandsQueue(matchIdCopy);
}

Queue<std::shared_ptr<Snapshot>>& LobbyResolver::getResponsesQueue() {
    return matchesMapMonitor.getResponsesQueue(matchIdCopy, clientId);
}
