#ifndef PATH_ELEMENTS_H
#define PATH_ELEMENTS_H 

#define DIR_UP 0
#define DIR_LEFT 1
#define DIR_RIGHT 2
#define DIR_DOWN 3

#define DIR_CURVE_UP_LEFT 4
#define DIR_CURVE_UP_RIGHT 5
#define DIR_CURVE_DOWN_LEFT 6
#define DIR_CURVE_DOWN_RIGHT 7
#define DIR_CURVE_LEFT_UP 8
#define DIR_CURVE_LEFT_DOWN 9
#define DIR_CURVE_RIGHT_UP 10
#define DIR_CURVE_RIGHT_DOWN 11

#define START_HORIZONTAL 14
#define START_VERTICAL 15

#define CHECKPOINT_HORIZONTAL 16
#define CHECKPOINT_VERTICAL 17

#define FINISH_HORIZONTAL 18
#define FINISH_VERTICAL 19

struct PathElement {
    int id;
    float x;
    float y;
    
    PathElement(int id, float x, float y)
        : id(id), x(x), y(y) {}

    PathElement() : id(-1), x(0), y(0) {}

    bool operator==(const PathElement& other) const {
        return id == other.id && x == other.x && y == other.y;
    }

    bool operator!=(const PathElement& other) const {
        return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream& os, const PathElement& p);
};

#endif
