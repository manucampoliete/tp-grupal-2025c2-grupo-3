#include "arrowitem.h"
#include <QDebug>
#include <QColor>
#include <iostream>

ArrowItem::ArrowItem(const QPixmap& pixmap, const QImage& mask)
        : QGraphicsPixmapItem(pixmap),
        collisionMask(mask)
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);

    setFlag(ItemSendsGeometryChanges, true);
    // 1. ESTABLECER OFFSET AL CENTRO: Crucial para que p.x(), p.y() representen
    // el centro del ítem, no su esquina (0,0).
    setOffset(-pixmap.width() / 2, -pixmap.height() / 2);
}

QVariant ArrowItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    std::cout << "--- ItemChange llamado. Tipo de cambio: " << change << std::endl;
    if (change == ItemPositionChange) {
        QPointF newPosF = value.toPointF();

        // --- DEBUG GLOBAL ---
        // Imprime la posición de la escena CADA vez que intentas mover la flecha
        std::cout << "\n--- Movimiento Propuesto ---" << std::endl;
        std::cout << "SCENE POS (float): (" << newPosF.x() << ", " << newPosF.y() << ")" << std::endl;
        // --------------------

                // --- 1. Definir los puntos a verificar (Centro y 4 esquinas del ítem) ---
        QList<QPoint> checkPoints;
        QSize itemSize = pixmap().size();

        // El offset hace que newPosF sea el centro.
        checkPoints.append(newPosF.toPoint()); // P0: Centro

        // Puntos de esquina ajustados por la mitad del tamaño (itemSize.width()/2)
        checkPoints.append((newPosF - QPointF(itemSize.width() / 2, itemSize.height() / 2)).toPoint()); // P1: Superior Izquierda
        checkPoints.append((newPosF + QPointF(itemSize.width() / 2, itemSize.height() / 2)).toPoint()); // P2: Inferior Derecha

        // --- Bucle de Verificación por Píxel ---
        int pointIndex = 0;
        for (const QPoint& p : checkPoints)
        {
            // Omitimos la verificación de fuera de límites para centrarnos en el color
            if (p.x() >= 0 && p.y() >= 0 && p.x() < collisionMask.width() && p.y() < collisionMask.height())
            {
                QColor pixel = collisionMask.pixelColor(p);
                const int redValue = pixel.red();
                const int blackThreshold = 200; // Umbral alto para detectar casi cualquier gris/negro

                // --- DEBUG DE PÍXEL ---
                std::cout << "Punto P" << pointIndex << " [MASCARA COORD]: (" << p.x() << ", " << p.y() <<
                        ") RED VALUE: " << redValue << std::endl;
                // --------------------

                if (redValue < blackThreshold)
                {
                    // Si encontramos UNA colisión, bloqueamos el movimiento inmediatamente.
                    std::cout << "❌ COLISIÓN DETECTADA en P" << pointIndex << " (Valor Rojo < " << blackThreshold << ")" << std::endl;
                    return pos();
                }
            }
            pointIndex++;
        }

        // Si ningún punto colisionó, el movimiento está permitido
        return QGraphicsPixmapItem::itemChange(change, value);
    }

    return QGraphicsPixmapItem::itemChange(change, value);
}
