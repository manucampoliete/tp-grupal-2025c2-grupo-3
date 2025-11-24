#ifndef MATCHES_MAP_MONITOR_H
#define MATCHES_MAP_MONITOR_H

#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "../gameLogic/game.h"

class MatchesMapMonitor {
private:
    std::map<MatchID, std::unique_ptr<Game>> matchMap;
    std::mutex mtx;
    MatchID nextMatchId;

public:
    /**
     * Constructor
     */
    MatchesMapMonitor();

    /**
     * Creates a new match, adds the player as the first player in the match,
     * and returns the MatchID of the created match.
     */
    MatchID createMatch(ClientID clientId, const std::string& username, CarID carId);

    /**
     * Adds the player to an existing match with the given MatchID.
     * Returns true if the player was added successfully, false otherwise.
     */
    bool joinMatch(ClientID clientId, MatchID matchId, const std::string& username, CarID carId);

    /**
     * Starts the match with the given MatchID.
     * Returns true if the match was started successfully, false otherwise.
     */
    bool startMatch(MatchID matchId);

    /**
     * Reaps dead matches from the match map.
     */
    void reapDeadMatches();

    /**
     * Returns a reference to the client commands queue for the match with the given MatchID.
     */
    Queue<std::unique_ptr<Command>>& getClientCommandsQueue(MatchID matchId);

    /**
     * Returns a reference to the responses queue for the given client in the match with the given
     * MatchID.
     */
    Queue<std::shared_ptr<Snapshot>>& getResponsesQueue(MatchID matchId, ClientID clientId);

    /**
     * Destructor: stops and joins all matches.
     */
    ~MatchesMapMonitor();
};

#endif  // MATCHES_MAP_MONITOR_H
