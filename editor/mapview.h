#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

class MapView : public QGraphicsView {
    Q_OBJECT

public:
    explicit MapView(QGraphicsScene* scene, QWidget* parent = nullptr);

    QSize sizeHint() const override;
    void setMaskAndPixmap(const QImage& mask, const QPixmap& arrowPixmap);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;

private:
    QImage collisionMask;
    QPixmap arrowPixmap;
};

#endif
