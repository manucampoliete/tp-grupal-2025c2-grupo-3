#ifndef SERVER_MESSAGE_H
#define SERVER_MESSAGE_H

#include <cstdint>
#include <variant>
#include <vector>

#include "gameData.h"
#include "../../common/messages/snapshot.h"


struct CountdownMessage {
    uint8_t number;
};

struct RaceInfoMessage {
    RaceInfo info;
};

struct RaceStartMessage {
    //nada
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

struct StatsCountdownMessage {
    uint8_t number;
};

struct ModificationPhaseMessage {
    std::vector<CarProperties> properties;
};

struct ModCountdownMessage {
    uint8_t number;
};

struct GameEndMessage {
    FinalResults results;
};

using ServerMessage = std::variant<
    Snapshot,
    RaceInfoMessage,
    RaceStartMessage,
    CountdownMessage,
    CollisionMessage,
    PlayerDiedMessage,
    RaceEndMessage,
    StatsCountdownMessage,
    ModificationPhaseMessage,
    ModCountdownMessage,
    GameEndMessage
>;

#endif  // SERVER_MESSAGE_H