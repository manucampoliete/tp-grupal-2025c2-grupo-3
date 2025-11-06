#ifndef MOVE_REQUEST_H
#define MOVE_REQUEST_H

struct MoveRequest {
    bool up;
    bool down;
    bool left;
    bool right;

    MoveRequest(bool up, bool down, bool left, bool right)
        : up(up), down(down), left(left), right(right) {}
};

#endif  // MOVE_REQUEST_H