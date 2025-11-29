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

struct LoadedHintData {
    ElementDirection direction;
    double x;
    double y;
};

struct LoadedSegmentData {
    ElementType cpType;
    ElementDirection cpDirection;
    double cpX;
    double cpY;
    std::vector<LoadedHintData> hints;
};

MapPaths getMapPaths(int cityId);

class MapEditor : public QWidget {
    Q_OBJECT

public:
    explicit MapEditor(int cityId, QWidget* parent = nullptr);
    explicit MapEditor(int cityId, const QString& filePath, QWidget* parent = nullptr);

    void processNewElement(MapElement* newElement);
    void loadMapForEditing(const QString& filename);

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

    void serializeToYaml(const QString& filename);
    std::vector<LoadedSegmentData> deserializeFromYaml(const QString& filename);
    void setupLoadedCircuit(const std::vector<LoadedSegmentData>& loadedSegments);
    MapElement* recreateElement(ElementType type, ElementDirection direction, double x, double y);
    void setupMapAssets(int cityId);
};

#endif
