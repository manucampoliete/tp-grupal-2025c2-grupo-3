#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

class MapEditor;

class MapView : public QGraphicsView {
    Q_OBJECT

public:
    explicit MapView(QGraphicsScene* scene, QWidget* parent = nullptr);

    QSize sizeHint() const override;
    void setResources(const QImage& mask);
    void setMapEditorParent(MapEditor* parentEditor);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;

private:
    MapEditor* mapEditorParent;
    QImage collisionMask;
    QPixmap tempPixmap;
};

#endif
