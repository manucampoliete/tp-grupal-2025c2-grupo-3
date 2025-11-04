#ifndef VECTOR_2D_H
#define VECTOR_2D_H

#include <cstdint>

struct Vector2D {
    int16_t x;
    int16_t y;

    Vector2D(int16_t x, int16_t y) : x(x), y(y) {}
    Vector2D() : x(0), y(0) {}
};

#endif  // VECTOR_2D_H
