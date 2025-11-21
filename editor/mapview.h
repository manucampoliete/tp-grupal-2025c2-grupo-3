#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

class MapView : public QGraphicsView {
    Q_OBJECT

public:
    explicit MapView(QGraphicsScene* scene, QWidget* parent = nullptr);

protected:
    void wheelEvent(QWheelEvent* event) override;
    //void dragEnterEvent(QDragEnterEvent* event) override; // Nuevo
    //void dropEvent(QDropEvent* event) override; // Nuevo
};

#endif
