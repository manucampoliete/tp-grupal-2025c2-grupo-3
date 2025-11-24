#ifndef COLLISION_BITS_H
#define COLLISION_BITS_H

// category bits (donde estoy)
#define CAR_LOW_LAYER 0x0001
#define CAR_HIGH_LAYER 0x0002
#define WALL_LOW_LAYER 0x0004
#define WALL_HIGH_LAYER 0x0008
#define SENSOR_LAYER 0x0010 

// mask bits (con qué colisiono)
#define MASK_CAR_LOW (CAR_LOW_LAYER | WALL_LOW_LAYER | SENSOR_LAYER)    // con qué chocan los autos de abajo
#define MASK_CAR_HIGH (CAR_HIGH_LAYER | WALL_HIGH_LAYER | SENSOR_LAYER) // con qué chocan los autos de arriba
#define MASK_WALL_LOW (CAR_LOW_LAYER)                                   // con que chocan las paredes de abajo
#define MASK_WALL_HIGH (CAR_HIGH_LAYER)                                 // con que chocan las paredes de arriba
#define MASK_SENSOR (CAR_LOW_LAYER | CAR_HIGH_LAYER)                    // con que chocan los sensores

#define IS_SENSOR true

#endif
