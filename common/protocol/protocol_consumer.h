#ifndef PROTOCOL_CONSUMER_H
#define PROTOCOL_CONSUMER_H

#include "../common/utils/active_directions.h"
#include "../../server/types.h"

#include <cstdint>

/**
 * An interface for classes that consume protocol events.
 * Classes that implement this interface can handle various events
 * defined by the protocol, such as creating or joining matches,
 * starting matches, and moving in the game.
 */
class ProtocolConsumer {
private:
    ClientID client_id;

protected:
    ClientID get_client_id() const {
        return client_id;
    }

public:
    ProtocolConsumer(ClientID client_id) : client_id(client_id) {}

    // Lobby events
    virtual void on_create_match(const std::string& username, uint8_t car_id) = 0;
    virtual void on_join_match(uint16_t match_id, const std::string& username, uint8_t car_id) = 0;
    virtual void on_start_match() = 0;

    // Game events
    virtual void on_move(ActiveDirections directions) = 0;

    // Future: add more as needed

    virtual ~ProtocolConsumer() = default;
};

#endif // PROTOCOL_CONSUMER_H
