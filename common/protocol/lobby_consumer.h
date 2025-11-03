#ifndef LOBBY_CONSUMER_H
#define LOBBY_CONSUMER_H

#include "protocol_consumer.h"

class LobbyConsumer : public ProtocolConsumer {
private:
    MatchesMapMonitor& matches_map_monitor;
    Receiver& receiver;
    ServerProtocol& protocol;

public:
    LobbyConsumer(ClientID client_id, MatchesMapMonitor& matches_map_monitor, Receiver& receiver, ServerProtocol& protocol) : 
        ProtocolConsumer(client_id),
        matches_map_monitor(matches_map_monitor), 
        receiver(receiver), 
        protocol(protocol) {}

    void on_create_match(const std::string& username, uint8_t car_id) override {
        MatchID match_id;
        auto [commands_q, responses_q] = matches_map_monitor.create_match(get_client_id(), username, car_id, match_id);
        receiver.set_client_commands_queue(&commands_q);
        receiver.set_responses_queue(&responses_q);
        protocol.send_created(match_id);
    }

    void on_join_match(uint16_t match_id, const std::string& username, uint8_t car_id) override {
        auto [commands_q, responses_q] = matches_map_monitor.join_match(get_client_id(), match_id, username, car_id);
        receiver.set_client_commands_queue(&commands_q);
        receiver.set_responses_queue(&responses_q);
        protocol.send_joined(true);
    }

    void on_start_match() override {
        matches_map_monitor.start_match(get_client_id());
        protocol.send_started(true);
    }

    void on_move(ActiveDirections) override {
        // no-op
    }

};

#endif // LOBBY_CONSUMER_H
