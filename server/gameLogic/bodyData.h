#ifndef BODY_DATA_H
#define BODY_DATA_H

#define LAYER_SWITCH_SENSOR 1

class Player;

struct BodyData {
    Player* player;     // Used if it's a player's body
    int sensorId;       // Used if it's a sensor (layer switch, checkpoints, NPC nodes...)

    BodyData(int sensorId) : player(nullptr), sensorId(sensorId) {}
    BodyData(Player* player) : player(player), sensorId(-1) {}
};

#endif
