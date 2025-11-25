#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include <QWidget>
#include <QGraphicsScene>
#include <QImage>

#include "mapview.h"
#include "mapelement.h"
#include "circuitsegment.h"

struct MapPaths {
    QString mapPath;
    QString maskPath;
};

MapPaths getMapPaths(int cityId);

class MapEditor : public QWidget {
    Q_OBJECT

public:
    explicit MapEditor(int cityId, QWidget* parent = nullptr);

    void processNewElement(MapElement* newElement);

private slots:
    void onSaveRequest();
    void onElementRemoved(MapElement* element);

private:
    MapView* view = nullptr;
    QGraphicsScene* scene = nullptr;
    QGraphicsPixmapItem* mapItem = nullptr;
    QImage collisionMask;
    std::vector<MapElement*> hintsInCurrentSegment;
    std::vector<CircuitSegment> circuitSegments;
    int cityMapID;

    void loadCity(int cityId);
    void serializeToYaml(const QString& filename);
};

#endif
