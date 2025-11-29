#include "matchesMapMonitor.h"

#define FIRST_MATCH_ID 0

MatchesMapMonitor::MatchesMapMonitor(const Config& config): 
    config(config), 
    matchMap(), 
    mtx(), 
    nextMatchId(FIRST_MATCH_ID) {}

MatchID MatchesMapMonitor::createMatch(ClientID clientId, const std::string& username,
                                       CarID carId) {
    std::lock_guard<std::mutex> lock(mtx);
    matchMap[nextMatchId] = std::make_unique<Game>(config);
    matchMap[nextMatchId]->addPlayer(clientId, username, carId);
    return nextMatchId++;
}

bool MatchesMapMonitor::joinMatch(ClientID clientId, MatchID matchId, const std::string& username,
                                  CarID carId) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = matchMap.find(matchId);
    if (it == matchMap.end()) {
        return false;
    }
    return it->second->addPlayer(clientId, username, carId);
}

bool MatchesMapMonitor::startMatch(MatchID matchId) {
    std::lock_guard<std::mutex> lock(mtx);
    matchMap[matchId]->start();
    return true;
}

void MatchesMapMonitor::reapDeadMatches() {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto it = matchMap.begin(); it != matchMap.end();) {
        if (it->second->isDead()) {
            it = matchMap.erase(it);
        } else {
            ++it;
        }
    }
}

Queue<std::unique_ptr<Command>>& MatchesMapMonitor::getClientCommandsQueue(MatchID matchId) {
    std::lock_guard<std::mutex> lock(mtx);
    return matchMap[matchId]->getClientCommandsQueue();
}

Queue<std::shared_ptr<Snapshot>>& MatchesMapMonitor::getResponsesQueue(MatchID matchId,
                                                                       ClientID clientId) {
    std::lock_guard<std::mutex> lock(mtx);
    return matchMap[matchId]->getResponsesQueue(clientId);
}

MatchesMapMonitor::~MatchesMapMonitor() {
    std::lock_guard<std::mutex> lock(mtx);  // Necessary?
    matchMap.clear();
}
