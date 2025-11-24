#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include <QWidget>
#include <QGraphicsScene>
#include <QImage>

#include "mapview.h"
#include "mapelement.h"
#include "circuitsegment.h"

class MapEditor : public QWidget {
    Q_OBJECT

public:
    explicit MapEditor(int cityId, QWidget* parent = nullptr);

    void processNewElement(MapElement* newElement);

private slots:
    void onSaveRequest();

private:
    MapView* view;
    QGraphicsScene* scene;
    QGraphicsPixmapItem* mapItem = nullptr;
    QImage collisionMask;
    std::vector<HintData> hintsInCurrentSegment;
    std::vector<CircuitSegment> circuitSegments;

    void loadCity(int cityId);
    void serializeToYaml(const QString& filename);
};

#endif
