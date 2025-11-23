#include "mapelement.h"
#include <QColor>
#include <iostream>

MapElement::MapElement(ElementType type, ElementDirection direction, const QPixmap& pixmap, const QImage& mask)
        : QGraphicsPixmapItem(pixmap),
        collisionMask(mask),
        elementType(type),
        elementDirection(direction)
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges, true);
    setOffset(-pixmap.width() / 2, -pixmap.height() / 2); // Centro
}

QVariant MapElement::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange) {
        QPointF newPosF = value.toPointF();
        QPoint p = newPosF.toPoint();

        if (p.x() >= 0 && p.y() >= 0 && p.x() < collisionMask.width() && p.y() < collisionMask.height())
        {
            QColor pixel = collisionMask.pixelColor(p);
            const int redValue = pixel.red();
            const int blackThreshold = 200;

            if (redValue < blackThreshold)
            {
                std::cout << "❌ COLISIÓN DETECTADA en CENTRO." << std::endl;
                return pos(); // Rechazar movimiento
            }
        }
        return QGraphicsPixmapItem::itemChange(change, value);
    }
    return QGraphicsPixmapItem::itemChange(change, value);
}

QString getAssetPath(ElementType type, ElementDirection direction)
{
    QString base = ":/media/assets/";

    switch (type) {
        case TYPE_HINT:
            switch (direction) {
                case DIR_UP: return base + "arrow_up.png";
                case DIR_LEFT: return base + "arrow_left.png";
                case DIR_RIGHT: return base + "arrow_right.png";
                case DIR_DOWN: return base + "arrow_down.png";
                case DIR_CURVE_UP_LEFT: return base + "curve_up_left.png";
                case DIR_CURVE_UP_RIGHT: return base + "curve_up_right.png";
                case DIR_CURVE_DOWN_LEFT: return base + "curve_down_left.png";
                case DIR_CURVE_DOWN_RIGHT: return base + "curve_down_right.png";
                case DIR_CURVE_LEFT_UP: return base + "curve_left_up";
                case DIR_CURVE_LEFT_DOWN: return base + "curve_left_down";
                case DIR_CURVE_RIGHT_UP: return base + "curve_right_up";
                case DIR_CURVE_RIGHT_DOWN: return base + "curve_right_down";
            }
            break;

        case TYPE_CHECKPOINT:
            return (direction == DIR_HORIZONTAL) ? base + "cp_horizontal.png" : base + "cp_vertical.png";

        case TYPE_START:
            return (direction == DIR_HORIZONTAL) ? base + "start_horizontal.png" : base + "start_vertical.png";

        case TYPE_FINISH:
            return (direction == DIR_HORIZONTAL) ? base + "finish_horizontal.png" : base + "finish_vertical.png";
    }

    qWarning() << "Error: Asset no encontrado para Tipo:" << type << "Dirección:" << direction;
    return QString();
}

QPixmap loadPixmapForElement(ElementType type, ElementDirection direction)
{
    QString path = getAssetPath(type, direction);

    if (path.isEmpty()) {
        return QPixmap();
    }

    QPixmap originalPixmap(path);
    if (originalPixmap.isNull()) {
        qWarning() << "Error CRÍTICO: No se pudo cargar el archivo:" << path;
        return QPixmap();
    }

    const int desiredSize = 50;

    QPixmap scaledPixmap = originalPixmap.scaled(
            desiredSize, desiredSize,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );

    return scaledPixmap;
}
