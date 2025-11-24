#include "mapview.h"
#include "mapelement.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QColor>
#include <iostream>

MapView::MapView(QGraphicsScene* scene, QWidget* parent)
        : QGraphicsView(scene, parent)
{
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setAcceptDrops(true);
}

QSize MapView::sizeHint() const
{
    return QSize(1000, 700);
}

void MapView::wheelEvent(QWheelEvent* event)
{
    const double scaleFactor = 1.15;

    if (event->angleDelta().y() > 0)
        scale(scaleFactor, scaleFactor);
    else
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
}

void MapView::setResources(const QImage& mask)
{
    this->collisionMask = mask;
    setAcceptDrops(true);
}

void MapView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText() && event->mimeData()->text().startsWith("asset/element/")) {
        event->acceptProposedAction();
        std::cout << "DEBUG: Evento DragEnter recibido. MIME: " << event->mimeData()->text().toStdString() << std::endl;
    } else {
        QGraphicsView::dragEnterEvent(event);
    }
}

void MapView::dropEvent(QDropEvent *event)
{
    QString mimeText = event->mimeData()->text();

    if (mimeText.startsWith("asset/element/")) {
        event->acceptProposedAction();
        std::cout << "DEBUG: Evento Drop recibido." << std::endl;

        QPointF scenePoint = mapToScene(event->pos());
        QPoint p = scenePoint.toPoint();

        bool outOfBounds = p.x() < 0 || p.y() < 0 || p.x() >= collisionMask.width() || p.y() >= collisionMask.height();
        bool isCollision = false;

        if (!outOfBounds) {
            QColor pixel = collisionMask.pixelColor(p);
            const int redValue = pixel.red();
            const int blackThreshold = 200;

            if (redValue < blackThreshold) {
                isCollision = true;
            }
        }

        if (outOfBounds || isCollision) {
            std::cout << (isCollision ? "❌ Drop RECHAZADO: No es una zona de calle." : "⚠️ Drop RECHAZADO: Fuera de los límites del mapa.") << std::endl;
            event->ignore();
            return;
        }

        QStringList parts = mimeText.split('/');
        ElementType type = static_cast<ElementType>(parts.value(2).toInt());
        ElementDirection direction = static_cast<ElementDirection>(parts.value(3).toInt());

        QPixmap elementPixmap = loadPixmapForElement(type, direction);

        MapElement* element = new MapElement(type, direction, elementPixmap, collisionMask);
        element->setPos(scenePoint);
        scene()->addItem(element);

        std::cout << "✅ Drop ACEPTADO: Tipo " << type << std::endl;
    } else {
        QGraphicsView::dropEvent(event);
    }
}
void MapView::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasText() && event->mimeData()->text().startsWith("asset/element/")) {
        event->acceptProposedAction();
    } else {
        QGraphicsView::dragMoveEvent(event);
    }
}
