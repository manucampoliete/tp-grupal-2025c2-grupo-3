#ifndef COLLISION_BITS_H
#define COLLISION_BITS_H

// Cathegory bits (where I am)
#define CAR_LOW_LAYER 0x0001
#define CAR_HIGH_LAYER 0x0002
#define WALL_LOW_LAYER 0x0004
#define WALL_HIGH_LAYER 0x0008
#define SENSOR_LAYER 0x0010 

// Mask bits (what I collide with)
#define MASK_CAR_LOW (CAR_LOW_LAYER | WALL_LOW_LAYER | SENSOR_LAYER)    // What the low layer cars collide with
#define MASK_CAR_HIGH (CAR_HIGH_LAYER | WALL_HIGH_LAYER | SENSOR_LAYER) // What the high layer cars collide with
#define MASK_WALL_LOW (CAR_LOW_LAYER)                                   // What the low layer walls collide with
#define MASK_WALL_HIGH (CAR_HIGH_LAYER)                                 // What the high layer walls collide with
#define MASK_SENSOR (CAR_LOW_LAYER | CAR_HIGH_LAYER)                    // What the sensors collide with
#define IS_SENSOR true

#endif
