#ifndef PROTOCOL_CONSTANTS_H
#define PROTOCOL_CONSTANTS_H

// temporal del dummy protocol
#define COD_MOVE        0x00
#define COD_POSITIONS   0x01

#define SEND_USERNAME       0x01
#define SEND_INITIAL_INFO   0x02
#define SEND_CREATE         0x03
#define SEND_CREATED        0x04
#define SEND_JOIN           0x05
#define SEND_JOINED         0x06
#define SEND_START          0x07
#define SEND_STARTED        0x08

#define SEND_MOVE_STATE     0x09
#define SEND_SNAPSHOT       0x0A

// Cliente → Servidor
#define MSG_MOVE            0x09  // enviar input de movimiento (up/down/left/right)
#define MSG_MODIFY_CAR      0x0D  // enviar modificaciones de auto
#define MSG_DISCONNECT      0x0F  // desconexión del cliente

// Servidor → Cliente
#define MSG_ASSIGN_ID       0x10  // servidor asigna id al cliente
#define MSG_SNAPSHOT        0x0A  // broadcast del estado del juego
#define MSG_COUNTDOWN       0x12  // countdown antes de empezar
#define MSG_RACE_START      0x13  // señal de inicio de carrera
#define MSG_CHECKPOINT      0x14  // jugador cruzó un checkpoint
#define MSG_COLLISION       0x15  // notificación de colisión
#define MSG_PLAYER_DIED     0x16  // un jugador murio (salud = 0?)
#define MSG_RACE_END        0x17  // terminó la carrera (resultados)
#define MSG_MOD_PHASE       0x18  // fase de modificación de auto
#define MSG_GAME_END        0x19  // partida completa terminada (ganador)


#endif // PROTOCOL_CONSTANTS_H
