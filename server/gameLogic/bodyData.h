#ifndef BODY_DATA_H
#define BODY_DATA_H

#define LAYER_SWITCH_SENSOR 1

class Player;

struct BodyData {
    Player* player; // si es un auto
    int sensorId; // si es un sensor (cambio de capa, checkpoints, nodos de NPC...)
};

#endif
