#ifndef BODY_DATA_H
#define BODY_DATA_H

#include "collisions/pathLoader.h"
#include "collisions/graphLoader.h"

#define NOT_A_SENSOR 0
#define LAYER_SWITCH_SENSOR 1
#define CHECKPOINT_SENSOR 2
#define GRAPH_NODE_SENSOR 3

class Player;

struct BodyData {
    Player* player;     // Used if it's a player's body
    int sensorId;       // Used if it's a sensor (layer switch, checkpoints, NPC nodes...)
    PathElement element;    // Used if it's a checkpoint sensor
    GraphNode node;     // Used if it's an NPC graph sensor

    BodyData(int sensorId) : player(nullptr), sensorId(sensorId) {}
    BodyData(Player* player) : player(player), sensorId(NOT_A_SENSOR) {}
    BodyData(int sensorId, PathElement element) : player(nullptr), sensorId(sensorId), element(element) {}
    BodyData(int sensorId, GraphNode node) : player(nullptr), sensorId(sensorId), node(node) {}
};

#endif
