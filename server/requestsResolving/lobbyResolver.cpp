#include "lobbyResolver.h"

LobbyResolver::LobbyResolver(ClientID clientId, MatchesMapMonitor& matchesMapMonitor, const std::vector<CarInfo>& carsInfo):
        clientId(clientId),
        matchesMapMonitor(matchesMapMonitor),
        carsInfo(carsInfo),
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

void LobbyResolver::handleStartMatch(/**ServerLobbyProtocol& protocol*/) {
    bool started = matchesMapMonitor.startMatch(matchIdCopy);
    /** protocol.sendStarted(started); */
    inLobbyPhase = !started;
}

bool LobbyResolver::isInLobbyPhase() const { return inLobbyPhase; }

const std::vector<CarInfo>& LobbyResolver::getCarsInfo() {
    return carsInfo;
}

Queue<std::unique_ptr<Command>>& LobbyResolver::getClientCommandsQueue() {
    return matchesMapMonitor.getClientCommandsQueue(matchIdCopy);
}

Queue<std::shared_ptr<Snapshot>>& LobbyResolver::getResponsesQueue() {
    return matchesMapMonitor.getResponsesQueue(matchIdCopy, clientId);
}
