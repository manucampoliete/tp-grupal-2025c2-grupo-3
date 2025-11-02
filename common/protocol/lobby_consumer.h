#ifndef LOBBY_CONSUMER_H
#define LOBBY_CONSUMER_H

#include "protocol_consumer.h"

class LobbyConsumer : public ProtocolConsumer {
private:
    // LobbyController& lobby_controller;

public:
    LobbyConsumer(ClientID client_id/*, LobbyController& lobby_controller*/) : ProtocolConsumer(client_id)/*, lobby_controller(lobby_controller)*/ {}

    void on_create_match(uint8_t car_id) override {
        /* lobby_controller.create_match(get_client_id(), car_id); */
    }

    void on_join_match(uint16_t match_id, uint8_t car_id) override {
        /* lobby_controller.join_match(get_client_id(), match_id, car_id); */
    }

    void on_start_match() override {
        /* lobby_controller.start_match(get_client_id()); */
    }

    void on_move(ActiveDirections) override {
        // no-op
    }

};

#endif // LOBBY_CONSUMER_H
