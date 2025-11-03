#include "receiver.h"
#include "lobby_consumer.h"
#include "game_consumer.h"

#include <syslog.h>

Receiver::Receiver(ServerProtocol& protocol, MatchesMapMonitor& matches_map_monitor, ClientID client_id, Sender& sender):
        protocol(protocol),
        matches_map_monitor(matches_map_monitor),
        client_id(client_id),
        sender(sender),
        client_commands_q(nullptr),
        responses_q(nullptr) {}

void Receiver::run() {
    try {
        bool match_started = false;
        
        // First, create a LobbyConsumer to handle lobby-related commands
        LobbyConsumer lobby_consumer(client_id, matches_map_monitor, *this, protocol);
        while (!match_started && should_keep_running()) {
            match_started = protocol.consume_one(lobby_consumer);
        }

        sender.set_responses_queue(responses_q);
        sender.start();

        // Now, create a GameConsumer to handle in-game commands
        GameConsumer game_consumer(client_id, *client_commands_q);
        while (should_keep_running()) {
            protocol.consume_one(game_consumer);
        }
        
    } catch (const std::exception& e) {
        syslog(LOG_ERR, "[Error] Receiver: Exception in client %u: %s", client_id, e.what());
    }
}

void Receiver::set_client_commands_queue(Queue<std::unique_ptr<Command>>* client_commands_q) {
    this->client_commands_q = client_commands_q;
}

void Receiver::set_responses_queue(Queue<Snapshot>* responses_q) {
    this->responses_q = responses_q;
}
