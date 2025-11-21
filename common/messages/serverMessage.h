#ifndef SERVER_MESSAGE_H
#define SERVER_MESSAGE_H

#include <cstdint>
#include <variant>

#include "gameData.h"
#include "snapshot.h"


struct CountdownMessage {
    uint8_t number;
};

struct RaceStartMessage {};

struct CheckpointMessage {
    uint8_t checkpointId;
};

struct CollisionMessage {
    CollisionData data;
};

struct PlayerDiedMessage {
    uint16_t playerId;
};

struct RaceEndMessage {
    RaceResults results;
};

struct ModificationPhaseMessage {
    CarProperties properties;
};

struct GameEndMessage {
    FinalResults results;
};

using ServerMessage = std::variant<
    Snapshot,
    CountdownMessage,
    RaceStartMessage,
    CheckpointMessage,
    CollisionMessage,
    PlayerDiedMessage,
    RaceEndMessage,
    ModificationPhaseMessage,
    GameEndMessage
>;

#endif  // SERVER_MESSAGE_H