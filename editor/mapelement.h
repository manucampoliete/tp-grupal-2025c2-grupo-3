#ifndef MAPELEMENT_H
#define MAPELEMENT_H

#include <QGraphicsPixmapItem>
#include <QImage>
#include <QString>

// --- ENUMS DE IDENTIFICACIÓN ---
enum ElementType {
    TYPE_HINT,
    TYPE_CHECKPOINT,
    TYPE_START,
    TYPE_FINISH
};

enum ElementDirection {
    // TIPOS BASE (Tus 4 direcciones cardinales)
    DIR_UP = 0,    // ⬆️ Flecha hacia arriba/adelante (Ya implementada)
    DIR_LEFT = 1,       // ⬅️ Flecha hacia izquierda
    DIR_RIGHT = 2,      // ➡️ Flecha hacia derecha
    DIR_DOWN = 3,   // ⬇️ Flecha hacia abajo

    // TIPOS DE CURVA DE 90 GRADOS (Cuatro esquinas)
    DIR_CURVE_UP_LEFT = 4,    // ↖️ Giro superior hacia la izquierda (Cuadrante superior-izquierdo)
    DIR_CURVE_UP_RIGHT = 5,   // ↗️ Giro superior hacia la derecha
    DIR_CURVE_DOWN_LEFT = 6,  // ↙️ Giro inferior hacia la izquierda
    DIR_CURVE_DOWN_RIGHT = 7, // ↘️ Giro inferior hacia la derecha

    DIR_CURVE_LEFT_UP = 8,
    DIR_CURVE_LEFT_DOWN = 9,
    DIR_CURVE_RIGHT_UP = 10,
    DIR_CURVE_RIGHT_DOWN = 11,

    // TIPOS DE PROPÓSITO GENERAL (Usados por CP/Start/Finish)
    DIR_HORIZONTAL = 12,
    DIR_VERTICAL = 13
};

QPixmap loadPixmapForElement(ElementType type, ElementDirection direction);

// --- CLASE BASE DE TODOS LOS ELEMENTOS DEL EDITOR ---
class MapElement : public QGraphicsPixmapItem {
public:
    MapElement(ElementType type, ElementDirection direction, const QPixmap& pixmap, const QImage& mask);

            // Getters para guardar en JSON/YAML
    ElementType getElementType() const { return elementType; }
    ElementDirection getElementDirection() const { return elementDirection; }



protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    QImage collisionMask;
    ElementType elementType;
    ElementDirection elementDirection;
};

// Función de utilidad para obtener la ruta del asset
QString getAssetPath(ElementType type, ElementDirection direction);

#endif
