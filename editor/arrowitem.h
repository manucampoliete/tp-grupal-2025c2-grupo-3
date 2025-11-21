#ifndef ARROWITEM_H
#define ARROWITEM_H

#include <QGraphicsPixmapItem>
#include <QImage>

class ArrowItem : public QGraphicsPixmapItem {
public:
    ArrowItem(const QPixmap& pixmap, const QImage& mask);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    QImage collisionMask;
};

#endif
