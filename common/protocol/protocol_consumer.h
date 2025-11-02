#ifndef PROTOCOL_CONSUMER_H
#define PROTOCOL_CONSUMER_H

#include <cstdint>

/**
 * An interface for classes that consume protocol events.
 * Classes that implement this interface can handle various events
 * defined by the protocol, such as creating or joining matches,
 * starting matches, and moving in the game.
 */
class ProtocolConsumer {
public:
    virtual ~ProtocolConsumer() = default;

    // Lobby events
    virtual void on_create_match(uint8_t car_id) = 0;
    virtual void on_join_match(uint16_t match_id, uint8_t car_id) = 0;
    virtual void on_start_match() = 0;

    // Game events
    virtual void on_move(uint8_t directions) = 0;

    // Future: add more as needed
};

#endif // PROTOCOL_CONSUMER_H
