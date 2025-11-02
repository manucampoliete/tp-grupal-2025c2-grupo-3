#ifndef ACTIVE_DIRECTIONS_H
#define ACTIVE_DIRECTIONS_H

class ActiveDirections {
public:
    bool up;
    bool down;
    bool left;
    bool right;

    ActiveDirections(bool up = false, bool down = false, bool left = false, bool right = false)
        : up(up), down(down), left(left), right(right) {}

    ActiveDirections(const ActiveDirections& other)
        : up(other.up), down(other.down), left(other.left), right(other.right) {}
};

#endif // ACTIVE_DIRECTIONS_H
