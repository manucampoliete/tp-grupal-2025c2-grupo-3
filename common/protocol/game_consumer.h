#ifndef GAME_CONSUMER_H
#define GAME_CONSUMER_H

#include "protocol_consumer.h"
#include "../server/commands/move_command.h"
#include "../common/utils/active_directions.h"
#include "../common/queue/queue.h"

#include <memory>

class GameConsumer : public ProtocolConsumer {
private:
    Queue<std::unique_ptr<Command>>& client_commands_q;

public:
    GameConsumer(ClientID client_id, Queue<std::unique_ptr<Command>>& client_commands_q) : 
        ProtocolConsumer(client_id), 
        client_commands_q(client_commands_q) {}

    void on_create_match(const std::string&, uint8_t) override {
        // no-op
    }

    void on_join_match(uint16_t, const std::string&, uint8_t) override {
        // no-op
    }

    void on_start_match() override {
        // no-op
    }

    void on_move(ActiveDirections directions) override {
        client_commands_q.push(std::make_unique<MoveCommand>(get_client_id(), directions));
    }
};

#endif // GAME_CONSUMER_H
