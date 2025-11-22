#include "mapview.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QColor>
#include <iostream>
#include "arrowitem.h"

MapView::MapView(QGraphicsScene* scene, QWidget* parent)
        : QGraphicsView(scene, parent)
{
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);  // arrastrar mapa con mouse
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

void MapView::setMaskAndPixmap(const QImage& mask, const QPixmap& arrowPixmap)
{
    this->collisionMask = mask;
    this->arrowPixmap = arrowPixmap;
    //setAcceptDrops(true);
}

void MapView::dragEnterEvent(QDragEnterEvent *event)
{
    std::cout << "DEBUG: Evento DragEnter recibido. MIME: " << event->mimeData()->text().toStdString() << std::endl;
    // Verifica si es el asset de flecha que creamos en ToolboxWidget
    if (event->mimeData()->hasText() && event->mimeData()->text() == "asset/hint_arrow") {
        event->acceptProposedAction();
    } else {
        QGraphicsView::dragEnterEvent(event);
    }
}

void MapView::dropEvent(QDropEvent *event)
{
    std::cout << "DEBUG: Evento Drop recibido." << std::endl;

    if (event->mimeData()->hasText() && event->mimeData()->text() == "asset/hint_arrow") {

        event->acceptProposedAction();

        // 1. Obtener la posición en la escena (sin zoom/scroll)
        QPointF scenePoint = mapToScene(event->pos());
        QPoint p = scenePoint.toPoint();

                // 2. VERIFICACIÓN DE RESTRICCIÓN (Mismo código que en ArrowItem::itemChange)

        if (p.x() >= 0 && p.y() >= 0 && p.x() < collisionMask.width() && p.y() < collisionMask.height())
        {
            QColor pixel = collisionMask.pixelColor(p);
            const int redValue = pixel.red();
            const int blackThreshold = 200;

            if (redValue < blackThreshold)
            {
                // Zona de Colisión (Edificio/Negro)
                std::cout << "❌ Drop RECHAZADO: No es una zona de calle." << std::endl;
                event->ignore();
                return;
            }
        }
        else if (p.x() < 0 || p.y() < 0 || p.x() >= collisionMask.width() || p.y() >= collisionMask.height())
        {
            std::cout << "⚠️ Drop RECHAZADO: Fuera de los límites del mapa." << std::endl;
            event->ignore();
            return;
        }

                // 3. DROP ACEPTADO: Crear el nuevo ArrowItem
        ArrowItem* arrow = new ArrowItem(arrowPixmap, collisionMask);

        // Posicionar el centro del ítem en el punto de drop
        arrow->setPos(scenePoint);
        scene()->addItem(arrow);

        std::cout << "✅ Drop ACEPTADO y ArrowItem creado en:" << scenePoint.x() << ", " << scenePoint.y() << std::endl;
        event->acceptProposedAction();
    } else {
        QGraphicsView::dropEvent(event);
    }
}

void MapView::dragMoveEvent(QDragMoveEvent *event)
{
    // Verifica continuamente si el elemento arrastrado es la flecha
    if (event->mimeData()->hasText() && event->mimeData()->text() == "asset/hint_arrow") {
        // Acepta la acción en la posición actual, lo que permite que el drop se dispare
        event->acceptProposedAction();
    } else {
        // Para cualquier otro drag, dejamos que la clase base decida
        QGraphicsView::dragMoveEvent(event);
    }
}
