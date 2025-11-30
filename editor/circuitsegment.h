#include "mapelement.h"
#include <vector>

class CircuitSegment {
public:
    MapElement* cpElementPtr;
    std::vector<MapElement*> segmentHints;
};
