#ifndef PROTOCOL_CONSTANTS_H
#define PROTOCOL_CONSTANTS_H

/**
 * Protocol message type constants
 */

#define SEND_USERNAME       0x01  // Not used for now
#define SEND_INITIAL_INFO   0x02  // Not used for now
#define SEND_CREATE         0x03
#define SEND_CREATED        0x04
#define SEND_JOIN           0x05
#define SEND_JOINED         0x06
#define SEND_START          0x07
#define SEND_STARTED        0x08
#define SEND_MOVE_STATE     0x09
#define SEND_SNAPSHOT       0x0A

#endif  // PROTOCOL_CONSTANTS_H
