#include "matchesMapMonitor.h"

#define FIRST_MATCH_ID 0

MatchesMapMonitor::MatchesMapMonitor(): matchMap(), mtx(), nextMatchId(FIRST_MATCH_ID) {}

MatchID MatchesMapMonitor::createMatch(ClientID clientId, const std::string& username,
                                       CarID carId) {
    std::lock_guard<std::mutex> lock(mtx);
    matchMap[nextMatchId] = std::make_unique<Game>();
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
    it->second->addPlayer(clientId, username, carId);
    return true;
}

bool MatchesMapMonitor::startMatch(MatchID matchId) {
    std::lock_guard<std::mutex> lock(mtx);
    matchMap[matchId]->start();
    return true;
}

void MatchesMapMonitor::reapDeadMatches() {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto it = matchMap.begin(); it != matchMap.end(); ) {
        if (it->second->isDead()) {
            it->second->join();
            it = matchMap.erase(it);
        } else {
            ++it;
        }
    }
}

void MatchesMapMonitor::stopAllMatches() {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto& pair : matchMap) {
        pair.second->stop();
    }
}

void MatchesMapMonitor::joinAllMatches() {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto& pair : matchMap) {
        pair.second->join();
    }
    matchMap.clear();
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
