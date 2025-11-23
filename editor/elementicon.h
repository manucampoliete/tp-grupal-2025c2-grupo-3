#ifndef ELEMENTICON_H
#define ELEMENTICON_H

#include <QWidget>
#include <QMouseEvent>
#include <QPixmap>
#include "mapelement.h"

class ElementIcon : public QWidget
{
    Q_OBJECT
public:
    ElementIcon(ElementType type, ElementDirection direction, QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const override { return QSize(50, 50); }

private:
    ElementType elementType;
    ElementDirection elementDirection;
    QPixmap iconPixmap;
};

#endif
