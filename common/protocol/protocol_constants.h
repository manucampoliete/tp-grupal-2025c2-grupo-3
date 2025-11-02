#ifndef PROTOCOL_CONSTANTS_H
#define PROTOCOL_CONSTANTS_H

#define SEND_USERNAME       0x00
#define SEND_INITIAL_INFO   0x01
#define SEND_CREATE         0x02
#define SEND_CREATED        0x03
#define SEND_JOIN           0x04
#define SEND_JOINED         0x05
#define SEND_START          0x06
#define SEND_STARTED        0x07
#define SEND_MOVE_STATE     0x08
#define SEND_SNAPSHOT       0x09

#endif // PROTOCOL_CONSTANTS_H

/*

// --- Códigos de Acción (Cliente -> Servidor) ---
#define COD_SEND_USERNAME 0x01
#define COD_SEND_CREATE 0x03
#define COD_SEND_JOIN 0x05
#define COD_SEND_START 0x07
#define COD_SEND_MOVE_STATE 0x09
#define COD_SEND_MODIFICATIONS 0x0D // nuevo: para mods

// --- Códigos de Acción (Servidor -> Cliente) ---
#define COD_SEND_INITIAL_INFO 0x02
#define COD_SEND_CREATED 0x04
#define COD_SEND_JOINED 0x06
#define COD_SEND_STARTED 0x08
#define COD_SEND_SNAPSHOT 0x0A
#define COD_SEND_RACE_RESULTS 0x0B  // nuevo: para stats
#define COD_SEND_MOD_PHASE 0x0C     // nuevo: para mods

*/

#endif  // PROTOCOL_CONSTANTS_H
