#ifndef MAPELEMENT_H
#define MAPELEMENT_H

#include <QGraphicsPixmapItem>
#include <QImage>
#include <QString>

enum ElementType {
    TYPE_HINT,
    TYPE_CHECKPOINT,
    TYPE_START,
    TYPE_FINISH
};

enum ElementDirection {
    DIR_UP = 0,
    DIR_LEFT = 1,
    DIR_RIGHT = 2,
    DIR_DOWN = 3,

    DIR_CURVE_UP_LEFT = 4,
    DIR_CURVE_UP_RIGHT = 5,
    DIR_CURVE_DOWN_LEFT = 6,
    DIR_CURVE_DOWN_RIGHT = 7,
    DIR_CURVE_LEFT_UP = 8,
    DIR_CURVE_LEFT_DOWN = 9,
    DIR_CURVE_RIGHT_UP = 10,
    DIR_CURVE_RIGHT_DOWN = 11,

    DIR_HORIZONTAL = 12,
    DIR_VERTICAL = 13
};

enum UnifiedId{
    START_HORIZONTAL = 14,
    START_VERTICAL = 15,
    
    CHECKPOINT_HORIZONTAL = 16,
    CHECKPOINT_VERTICAL = 17,
    
    FINISH_HORIZONTAL = 18,
    FINISH_VERTICAL = 19
};

struct ElementProperties {
    ElementType type;
    ElementDirection direction;
};

QPixmap loadPixmapForElement(ElementType type, ElementDirection direction);
QString typeToString(ElementType type);
QString directionToString(ElementDirection direction);
ElementType stringToType(const std::string& typeStr);
ElementDirection stringToDirection(const std::string& dirStr);
int mapElementToUnifiedId(ElementType type, ElementDirection direction);
ElementProperties mapUnifiedIdToTypeAndDirection(int unifiedId);


class MapElement : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    MapElement(ElementType type, ElementDirection direction, const QPixmap& pixmap, const QImage& mask);

    ElementType getElementType() const { return elementType; }
    ElementDirection getElementDirection() const { return elementDirection; }
    void deleteElement();

signals:
    void elementRemoved(MapElement* element);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QImage collisionMask;
    ElementType elementType;
    ElementDirection elementDirection;
};

QString getAssetPath(ElementType type, ElementDirection direction);

#endif
