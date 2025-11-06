#ifndef ACTIVE_DIRECTIONS_H
#define ACTIVE_DIRECTIONS_H

class ActiveDirections {
public:
    bool up;
    bool down;
    bool left;
    bool right;

    /**
     * Constructor
     */
    explicit ActiveDirections(bool up = false, bool down = false, bool left = false, bool right = false);

    /**
     * Copy constructor
     */
    ActiveDirections(const ActiveDirections& other);
};

#endif  // ACTIVE_DIRECTIONS_H
