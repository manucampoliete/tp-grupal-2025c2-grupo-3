#include "mapelement.h"
#include <vector>
#include <QPointF>

struct HintData {
    ElementDirection direction;
    QPointF position;
};

class CircuitSegment {
public:
    ElementType cpType;
    ElementDirection cpDirection;
    QPointF cpPosition;

    std::vector<HintData> segmentHints;
};
