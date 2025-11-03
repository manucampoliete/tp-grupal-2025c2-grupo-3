#ifndef MATCHES_MAP_MONITOR_H
#define MATCHES_MAP_MONITOR_H

#include "types.h"
#include "game.h"
#include "commands/command.h"
#include "../common/messages/snapshot.h"

#include <map>
#include <mutex>
#include <memory>
#include <utility>

class MatchesMapMonitor {
private:
    std::map<MatchID, std::unique_ptr<Game>> match_map;
    std::mutex mtx;
    MatchID next_match_id;

public:
    MatchesMapMonitor() : match_map(), mtx(), next_match_id(1) {}

    std::pair<Queue<std::unique_ptr<Command>>&, Queue<Snapshot>&> create_match(ClientID client_id, const std::string& username, uint8_t car_id, MatchID& match_id) {
        std::lock_guard<std::mutex> lock(mtx);

        match_id = next_match_id++;
        Game& game = *(match_map[match_id] = std::make_unique<Game>());

        game.add_player(client_id, username, car_id);

        return game.get_queues(client_id);
    }

    std::pair<Queue<std::unique_ptr<Command>>&, Queue<Snapshot>&> join_match(ClientID client_id, MatchID match_id, const std::string& username, uint8_t car_id) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = match_map.find(match_id);
        if (it == match_map.end()) {
            throw std::runtime_error("Match ID not found");
        }

        auto& game = *it->second;
        game.add_player(client_id, username, car_id);

        return game.get_queues(client_id);
    }

    void start_match(uint16_t match_id) {
        std::lock_guard<std::mutex> lock(mtx);
        match_map[match_id]->start();
        // broadcast?
    }
};

#endif // MATCHES_MAP_MONITOR_H
