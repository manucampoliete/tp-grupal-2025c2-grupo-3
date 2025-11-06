#include "matchesMapMonitor.h"

MatchesMapMonitor::MatchesMapMonitor(): matchMap(), mtx(), nextMatchId(1) {}

MatchID MatchesMapMonitor::createMatch(ClientID clientId, const std::string& username, CarID carId) {
    std::lock_guard<std::mutex> lock(mtx);
    matchMap[nextMatchId] = std::make_unique<Game>();
    matchMap[nextMatchId]->addPlayer(clientId, username, carId);
    return nextMatchId++;
}

bool MatchesMapMonitor::joinMatch(ClientID clientId, MatchID matchId, const std::string& username, CarID carId) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = matchMap.find(matchId);
    if (it == matchMap.end()) {
        return false;
    }
    it->second->addPlayer(clientId, username, carId);
    return true;
}

bool MatchesMapMonitor::startMatch(MatchID matchId) {
    std::lock_guard<std::mutex> lock(mtx);
    matchMap[matchId]->start();
    return true;
}

Queue<std::unique_ptr<Command>>& MatchesMapMonitor::getClientCommandsQueue(MatchID matchId) {
    std::lock_guard<std::mutex> lock(mtx);
    return matchMap[matchId]->getClientCommandsQueue();
}

Queue<std::shared_ptr<Snapshot>>& MatchesMapMonitor::getResponsesQueue(MatchID matchId, ClientID clientId) {
    std::lock_guard<std::mutex> lock(mtx);
    return matchMap[matchId]->getResponsesQueue(clientId);
}
