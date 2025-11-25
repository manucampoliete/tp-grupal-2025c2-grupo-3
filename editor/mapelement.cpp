#include "mapelement.h"
#include <QColor>
#include <iostream>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

MapElement::MapElement(ElementType type, ElementDirection direction, const QPixmap& pixmap, const QImage& mask)
        : QObject(nullptr), QGraphicsPixmapItem(pixmap),
        collisionMask(mask),
        elementType(type),
        elementDirection(direction)
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges, true);
    setOffset(-pixmap.width() / 2, -pixmap.height() / 2); // Centro
}

QString typeToString(ElementType type) {
    switch (type) {
        case TYPE_HINT: return "HINT";
        case TYPE_START: return "START_POINT";
        case TYPE_FINISH: return "FINISH_LINE";
        case TYPE_CHECKPOINT: return "CHECKPOINT";
    }
    return "UNKNOWN";
}

QString directionToString(ElementDirection direction) {
    switch (direction) {
        case DIR_HORIZONTAL: return "HORIZONTAL";
        case DIR_VERTICAL:   return "VERTICAL";

        default: return QString::number(direction);
    }
}

ElementType stringToType(const std::string& typeStr) {
    if (typeStr == "HINT") return TYPE_HINT;
    if (typeStr == "CHECKPOINT") return TYPE_CHECKPOINT;
    if (typeStr == "START_POINT") return TYPE_START;
    if (typeStr == "FINISH_LINE") return TYPE_FINISH;

    return TYPE_HINT;
}

ElementDirection stringToDirection(const std::string& dirStr) {
    if (dirStr == "HORIZONTAL") return DIR_HORIZONTAL;
    if (dirStr == "VERTICAL") return DIR_VERTICAL;

    try {
        return static_cast<ElementDirection>(std::stoi(dirStr));
    } catch (const std::invalid_argument& e) {
        // Manejo de error si el string no es un número (devolver un valor seguro)
        return DIR_UP;
    } catch (const std::out_of_range& e) {
        return DIR_UP;
    }

    return DIR_HORIZONTAL;
}

void MapElement::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        std::cout << "DEBUG: Clic derecho detectado en elemento." << std::endl;
        deleteElement();
        return;
    }
    QGraphicsPixmapItem::mousePressEvent(event);
}

void MapElement::deleteElement() {
    emit elementRemoved(this);

    if (scene()) {
        scene()->removeItem(this);
    }

    this->deleteLater();
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

        case TYPE_CHECKPOINT: // <-- NUEVO CASO
            switch (direction) {
                case DIR_HORIZONTAL: return base + "cp_horizontal.png";
                case DIR_VERTICAL:   return base + "cp_vertical.png";
                default: break;
            }
            break;

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
