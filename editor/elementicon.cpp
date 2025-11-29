#include "elementicon.h"
#include <QDrag>
#include <QMimeData>
#include <QPainter>
#include <QDebug>

extern QPixmap loadPixmapForElement(ElementType type, ElementDirection direction);

ElementIcon::ElementIcon(ElementType type, ElementDirection direction, QWidget* parent): QWidget(parent), elementType(type), elementDirection(direction){
    iconPixmap = loadPixmapForElement(type, direction);
    setCursor(Qt::OpenHandCursor);
}

void ElementIcon::paintEvent(QPaintEvent* event){
    QPainter painter(this);
    painter.drawPixmap(rect(), iconPixmap, iconPixmap.rect());
    QWidget::paintEvent(event);
}

void ElementIcon::mousePressEvent(QMouseEvent* event){
    if (event->button() == Qt::LeftButton) {

        QDrag* drag = new QDrag(this);
        QMimeData* mimeData = new QMimeData();

        QString mimeString = QString("asset/element/%1/%2").arg(elementType).arg(elementDirection);

        mimeData->setText(mimeString);
        drag->setPixmap(iconPixmap);
        drag->setMimeData(mimeData);
        drag->exec(Qt::CopyAction);
    }
}
