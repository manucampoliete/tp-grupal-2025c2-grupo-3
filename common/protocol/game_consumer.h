#ifndef GAME_CONSUMER_H
#define GAME_CONSUMER_H

#include "protocol_consumer.h"

class GameConsumer : public ProtocolConsumer {
private:
    // GameController& game_controller;

public:
    GameConsumer(ClientID client_id/*, GameController& game_controller*/) : ProtocolConsumer(client_id)/*, game_controller(game_controller)*/ {}

    void on_create_match(uint8_t) override {
        // no-op
    }

    void on_join_match(uint16_t, uint8_t) override {
        // no-op
    }

    void on_start_match() override {
        // no-op
    }

    void on_move(ActiveDirections directions) override {
        /* game_controller.move_player(get_client_id(), directions); */
    }

};


#endif // GAME_CONSUMER_H
