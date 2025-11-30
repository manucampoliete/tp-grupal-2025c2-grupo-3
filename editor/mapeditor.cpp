#include "mapeditor.h"
#include "mapelement.h"
#include <QVBoxLayout>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QPixmap>
#include <QGraphicsScene>
#include <iostream>
#include <QDir>
#include <QFileDialog>
#include "toolbox.h"
#include <yaml-cpp/yaml.h>
#include <algorithm>
#include <QMessageBox>

MapPaths getMapPaths(int cityId) {
    MapPaths paths;

    switch (cityId) {
        case 0:
            paths.mapPath = ":/media/vice_city_map.png"; // ":/media/assets/liberty_city_map.png"
            paths.maskPath = ":/media/mask_vicecity.jpeg"; // ":/media/assets/mask_liberty_city.jpeg"
            break;

        case 1:
            paths.mapPath = ":/media/vice_city_map.png"; // ":/media/assets/san_andreas_map.png"
            paths.maskPath = ":/media/mask_vicecity.jpeg"; // ":/media/assets/mask_san_andreas.jpeg"
            break;

        case 2:
            paths.mapPath = ":/media/vice_city_map.png";
            paths.maskPath = ":/media/mask_vicecity.jpeg";
            break;

        default:
            paths.mapPath = ":/media/vice_city_map.png";
            paths.maskPath = ":/media/mask_vicecity.jpeg";
            break;
    }
    return paths;
}

MapEditor::MapEditor(int cityId, const QString& filePath, QWidget* parent): MapEditor(cityId, parent) {
    if (!filePath.isEmpty())
        loadMapForEditing(filePath);
}

MapEditor::MapEditor(int cityId, QWidget* parent): QWidget(parent), cityMapID(cityId) {
    scene = new QGraphicsScene(this);
    view = new MapView(scene, this);

    this->setMinimumSize(1280, 720);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setScene(scene);

    setupMapAssets(cityMapID);

    view->setMapEditorParent(this);
    view->setResources(collisionMask);

    Toolbox* toolbox = new Toolbox(this);
    toolbox->setFixedWidth(150);

    connect(toolbox, &Toolbox::saveClicked, this, &MapEditor::onSaveRequest);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(toolbox);
    mainLayout->addWidget(view);

    setLayout(mainLayout);
}

void MapEditor::loadMapForEditing(const QString& filename) {
    LoadedMapData loadedData = deserializeFromYaml(filename);

    if (loadedData.segments.empty()) {
        QMessageBox::critical(this, "Loading error", "No valid elements found in the YAML file.");
        return;
    }

    setupMapAssets(this->cityMapID);
    setupLoadedCircuit(loadedData);
}

void MapEditor::setupMapAssets(int cityId) {
    MapPaths selectedPaths = getMapPaths(cityId);
    QString mapPath = selectedPaths.mapPath;
    QString maskPath = selectedPaths.maskPath;

    QPixmap mapPixmap(mapPath);

    if (mapPixmap.isNull()) 
        return;

    if (mapItem)
        scene->removeItem(mapItem);

    mapItem = scene->addPixmap(mapPixmap);
    mapItem->setZValue(0);

    if (this->collisionMask.load(maskPath)) {
        QSize targetSize = mapPixmap.size();
        this->collisionMask = this->collisionMask.convertToFormat(QImage::Format_Grayscale8);

        if (this->collisionMask.size() != targetSize)
            this->collisionMask = this->collisionMask.scaled(targetSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        view->setResources(this->collisionMask);
    }

    view->fitInView(mapItem, Qt::KeepAspectRatio);
    const double initialZoomFactor = 30.0;
    view->scale(initialZoomFactor, initialZoomFactor);
    view->centerOn(mapItem);
}

void MapEditor::onSaveRequest(){
    QString filter = "Archivos de Mapa YAML (*.yaml *.yml)";
    QString defaultPath = QDir::homePath() + "/nuevo_mapa.yaml";

    QString fileName = QFileDialog::getSaveFileName(this,"Guardar Circuito de Carrera",defaultPath,filter);

    if (!fileName.isEmpty()) {
        if (!fileName.toLower().endsWith(".yaml") && !fileName.toLower().endsWith(".yml"))
            fileName += ".yaml";

        serializeToYaml(fileName);
    }
}

void MapEditor::serializeToYaml(const QString& filename){
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;

    emitter << YAML::Key << "map_id" << YAML::Value << this->cityMapID;

    emitter << YAML::Key << "spawns";
    emitter << YAML::Value << YAML::BeginSeq;
    for (const auto& spawn : mapSpawns) {
        if (!spawn) continue;

        emitter << YAML::BeginMap;
        emitter << YAML::Key << "type_id" << YAML::Value << mapElementToUnifiedId(spawn->getElementType(), spawn->getElementDirection());

        QPointF pos = spawn->pos();
        emitter << YAML::Key << "position";
        emitter << YAML::Value << YAML::BeginSeq << pos.x() << pos.y() << YAML::EndSeq;
        emitter << YAML::EndMap;
    }
    emitter << YAML::EndSeq;

    emitter << YAML::Key << "segments";
    emitter << YAML::Value << YAML::BeginSeq;

    for (const auto& segment : circuitSegments) {
        if (!segment.cpElementPtr) continue;

        MapElement* cp = segment.cpElementPtr;
        int unifiedCpId = mapElementToUnifiedId(cp->getElementType(), cp->getElementDirection());

        emitter << YAML::BeginMap;
        emitter << YAML::Key << "segment_start_type" << YAML::Value << unifiedCpId;

        QPointF finalCpPos = cp->pos();
        emitter << YAML::Key << "position";
        emitter << YAML::Value << YAML::BeginSeq << finalCpPos.x() << finalCpPos.y() << YAML::EndSeq;

        emitter << YAML::Key << "hints_to_next_cp";
        emitter << YAML::Value << YAML::BeginSeq;

        for (const auto& hintPtr : segment.segmentHints) {
            if (!hintPtr) continue;

            MapElement* hint = hintPtr;
            emitter << YAML::BeginMap;
            emitter << YAML::Key << "type_id" << YAML::Value << hint->getElementDirection();            
            QPointF finalHintPos = hint->pos();
            emitter << YAML::Key << "position";
            emitter << YAML::Value << YAML::BeginSeq << finalHintPos.x() << finalHintPos.y() << YAML::EndSeq;
            emitter << YAML::EndMap;
        }

        emitter << YAML::EndSeq;
        emitter << YAML::EndMap;
    }

    emitter << YAML::EndSeq;
    emitter << YAML::EndMap;

    QFile file(filename);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        file.write(emitter.c_str());
        file.close();
    }
}

void MapEditor::processNewElement(MapElement* newElement) {
    connect(newElement, &MapElement::elementRemoved, this, &MapEditor::onElementRemoved, Qt::QueuedConnection);
    if (newElement->getElementType() == TYPE_HINT) {
        this->hintsInCurrentSegment.push_back(newElement);
    } else if (newElement->getElementType() == TYPE_START || newElement->getElementType() == TYPE_CHECKPOINT || newElement->getElementType() == TYPE_FINISH) {
        CircuitSegment newSegment;
        newSegment.cpElementPtr = newElement;

        if (!this->circuitSegments.empty())
            this->circuitSegments.back().segmentHints = this->hintsInCurrentSegment;

        this->circuitSegments.push_back(newSegment);
        this->hintsInCurrentSegment.clear();
    } else if (newElement->getElementType() == TYPE_SPAWN) {
        this->mapSpawns.push_back(newElement);
    }
}

void MapEditor::onElementRemoved(MapElement* element) {
    if (element->getElementType() == TYPE_HINT) {
        auto& hints = this->hintsInCurrentSegment;

        hints.erase(std::remove_if(hints.begin(), hints.end(), [element](MapElement* ptr) { return ptr == element; }), hints.end());
    }

    for (auto& segment : this->circuitSegments) {
        auto& hints = segment.segmentHints;
        hints.erase(std::remove(hints.begin(), hints.end(), element), hints.end());
    }

    if (element->getElementType() != TYPE_HINT) {
        auto& segments = this->circuitSegments;
        segments.erase(std::remove_if(segments.begin(), segments.end(), [element](const CircuitSegment& s) { return s.cpElementPtr == element; }), segments.end());
    }
}

MapElement* MapEditor::recreateElement(ElementType type, ElementDirection direction, double x, double y) {
    QPixmap elementPixmap = loadPixmapForElement(type, direction);

    MapElement* element = new MapElement(type, direction, elementPixmap, this->collisionMask);

    QObject::connect(element, &MapElement::elementRemoved, this, &MapEditor::onElementRemoved, Qt::QueuedConnection);

    element->setPos(x, y);
    scene->addItem(element);

    return element;
}

LoadedMapData MapEditor::deserializeFromYaml(const QString& filename) {
    LoadedMapData result;

    try {
        YAML::Node root = YAML::LoadFile(filename.toStdString());

        if (!root["map_id"])
            throw std::runtime_error("Falta la clave 'map_id'.");

        this->cityMapID = root["map_id"].as<int>();

        if (root["spawns"] && root["spawns"].IsSequence()) {

            for (const auto& spawnNode : root["spawns"]) {

                if (!spawnNode.IsMap()) continue;

                if (!spawnNode["type_id"])
                    continue;
                
                int typeId = spawnNode["type_id"].as<int>();
                ElementProperties props = mapUnifiedIdToTypeAndDirection(typeId);

                const YAML::Node& pos = spawnNode["position"];
                if (!pos || !pos.IsSequence() || pos.size() < 2)
                    continue;

                double x = pos[0].as<double>();
                double y = pos[1].as<double>();

                LoadedHintData spawnData;
                spawnData.direction = props.direction;
                spawnData.x = x;
                spawnData.y = y;

                result.spawns.push_back(spawnData);
            }
        }

        const YAML::Node& segmentsNode = root["segments"];
        if (!segmentsNode || !segmentsNode.IsSequence())
            throw std::runtime_error("'segments' node is missing or is not a sequence.");

        for (const auto& segmentNode : segmentsNode) {
            if (!segmentNode.IsMap()) continue;

            LoadedSegmentData segment;

            if (!segmentNode["segment_start_type"])
                throw std::runtime_error("'segment_start_type' missing.");

            int unifiedId = segmentNode["segment_start_type"].as<int>();
            ElementProperties cpProps = mapUnifiedIdToTypeAndDirection(unifiedId);

            segment.cpType = cpProps.type;
            segment.cpDirection = cpProps.direction;

            const YAML::Node& pos = segmentNode["position"];
            if (!pos || !pos.IsSequence() || pos.size() < 2)
                continue;

            segment.cpX = pos[0].as<double>();
            segment.cpY = pos[1].as<double>();

            const YAML::Node& hintsNode = segmentNode["hints_to_next_cp"];
            if (hintsNode && hintsNode.IsSequence()) {

                for (const auto& hintNode : hintsNode) {
                    if (!hintNode.IsMap()) continue;

                    if (!hintNode["type_id"])
                        continue;

                    int hintUnifiedId = hintNode["type_id"].as<int>();
                    ElementProperties hintProps = mapUnifiedIdToTypeAndDirection(hintUnifiedId);

                    const YAML::Node& hpos = hintNode["position"];
                    if (!hpos || !hpos.IsSequence() || hpos.size() < 2)     
                        continue;
                    
                    LoadedHintData hint;
                    hint.direction = hintProps.direction;
                    hint.x = hpos[0].as<double>();
                    hint.y = hpos[1].as<double>();

                    segment.hints.push_back(hint);
                }
            }
            result.segments.push_back(segment);
        }
    } catch (const YAML::BadFile& e) {
        QMessageBox::critical(this, "YAML Error", QString("Could not open file: %1").arg(e.what()));
        return {};
    } catch (const YAML::Exception& e) {
        QMessageBox::critical(this, "Parsing error", QString("Error parsing YAML: %1").arg(e.what()));
        return {};
    } catch (const std::runtime_error& e) {
        QMessageBox::critical(this, "Structure error", QString("Structure error: %1").arg(e.what()));
        return {};
    }
    return result;
}

void MapEditor::setupLoadedCircuit(const LoadedMapData& loadedData) {
    this->circuitSegments.clear();
    this->hintsInCurrentSegment.clear();

    for (const auto& loadedSegment : loadedData.segments) {

        MapElement* cpElement = recreateElement(loadedSegment.cpType, loadedSegment.cpDirection, loadedSegment.cpX, loadedSegment.cpY);

        CircuitSegment newSegment;
        newSegment.cpElementPtr = cpElement;

        for (const auto& hintData : loadedSegment.hints) {
            MapElement* hintElement = recreateElement(TYPE_HINT, hintData.direction, hintData.x, hintData.y);
            newSegment.segmentHints.push_back(hintElement);
        }

        this->circuitSegments.push_back(newSegment);
    }

    this->mapSpawns.clear();

    for (const auto& spawn : loadedData.spawns) {
        MapElement* spawnElement = recreateElement(TYPE_SPAWN, spawn.direction, spawn.x, spawn.y);
        this->mapSpawns.push_back(spawnElement);
    }

    if (!this->circuitSegments.empty()) {
        CircuitSegment& lastSegment = this->circuitSegments.back();

        if (lastSegment.cpElementPtr && lastSegment.cpElementPtr->getElementType() == TYPE_FINISH) {
            MapElement* finishPtr = lastSegment.cpElementPtr;

            this->circuitSegments.pop_back();

            if (!this->circuitSegments.empty())
                this->hintsInCurrentSegment = this->circuitSegments.back().segmentHints;

            QObject::disconnect(finishPtr, nullptr, this, nullptr);
            if (finishPtr->scene()) finishPtr->scene()->removeItem(finishPtr);
            finishPtr->deleteLater();
        }
        else {
            this->hintsInCurrentSegment = this->circuitSegments.back().segmentHints;
        }
    }
}
