#include "mapview.h"

MapView::MapView(QGraphicsScene* scene, QWidget* parent)
        : QGraphicsView(scene, parent)
{
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);  // arrastrar mapa con mouse
}

void MapView::wheelEvent(QWheelEvent* event)
{
    const double scaleFactor = 1.15;

    if (event->angleDelta().y() > 0)
        scale(scaleFactor, scaleFactor);
    else
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
}
