#include "activeDirections.h"

ActiveDirections::ActiveDirections(bool up, bool down, bool left, bool right):
        up(up), down(down), left(left), right(right) {}

ActiveDirections::ActiveDirections(const ActiveDirections& other):
        up(other.up), down(other.down), left(other.left), right(other.right) {}

ActiveDirections& ActiveDirections::operator=(const ActiveDirections& other) {
    if (this == &other)
        return *this;

    up = other.up;
    down = other.down;
    left = other.left;
    right = other.right;

    return *this;
}
