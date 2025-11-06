#include "lobbyResolver.h"

LobbyResolver::LobbyResolver(ClientID clientId, MatchesMapMonitor& matchesMapMonitor):
            clientId(clientId),
            matchesMapMonitor(matchesMapMonitor),
            inLobbyPhase(true) {}

void LobbyResolver::handleCreateMatch(const std::string& username, uint8_t carId, ServerLobbyProtocol& protocol) {
    MatchID matchId = matchesMapMonitor.createMatch(clientId, username, carId);
    protocol.sendCreated(matchId);
}

void LobbyResolver::handleJoinMatch(MatchID matchId, const std::string& username, uint8_t carId,
                        ServerLobbyProtocol& protocol) {
    bool joined = matchesMapMonitor.joinMatch(clientId, matchId, username, carId);
    protocol.sendJoined(joined);
    inLobbyPhase = !joined;
}

void LobbyResolver::handleStartMatch(MatchID matchId, ServerLobbyProtocol& protocol) {
    bool started = matchesMapMonitor.startMatch(matchId);
    protocol.sendStarted(started);
    inLobbyPhase = !started;
}

bool LobbyResolver::isInLobbyPhase() const {
    return inLobbyPhase;
}

Queue<std::unique_ptr<Command>>& LobbyResolver::getClientCommandsQueue() {
    return matchesMapMonitor.getClientCommandsQueue(matchId);
}

Queue<std::shared_ptr<Snapshot>>& LobbyResolver::getResponsesQueue() {
    return matchesMapMonitor.getResponsesQueue(matchId);
}

