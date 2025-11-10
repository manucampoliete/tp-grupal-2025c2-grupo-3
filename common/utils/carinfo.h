#ifndef CARINFO_H
#define CARINFO_H

#include <string>

#include "../types/types.h"

struct CarInfo {
    CarID id;
    std::string name;
    uint16_t health;
    uint16_t speed;
};

#endif  // CARINFO_H
