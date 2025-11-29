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

MapEditor::MapEditor(int cityId, const QString& filePath, QWidget* parent): MapEditor(cityId, parent){
    if (!filePath.isEmpty())
        loadMapForEditing(filePath);
}

MapEditor::MapEditor(int cityId, QWidget* parent): QWidget(parent), cityMapID(cityId){
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
        std::cout << "Advertencia: La carga falló o el archivo YAML está vacío. No se continuará." << std::endl;
        QMessageBox::critical(this, "Error de Carga", "No se encontraron datos de carrera válidos en el archivo.");
        return;
    }

    setupMapAssets(this->cityMapID);
    setupLoadedCircuit(loadedData);

    std::cout << "✅ Carga de mapa completada. El editor está listo para continuar." << std::endl;
}

void MapEditor::setupMapAssets(int cityId) {
    MapPaths selectedPaths = getMapPaths(cityId);
    QString mapPath = selectedPaths.mapPath;
    QString maskPath = selectedPaths.maskPath;

    QPixmap mapPixmap(mapPath);
    if (mapPixmap.isNull()) {
        std::cout << "ERROR CRÍTICO: No se pudo cargar el mapa principal en: " << mapPath.toStdString() << std::endl;
        return;
    }

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
    } else {
        std::cout << "ERROR CRÍTICO: No se pudo cargar la máscara en: " << maskPath.toStdString() << std::endl;
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

        std::cout << "Iniciando serialización a: " << fileName.toStdString() << std::endl;

        serializeToYaml(fileName);
    } else {
        std::cout << "Guardado cancelado por el usuario." << std::endl;
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
        emitter << YAML::Key << "type_id" << YAML::Value 
                << mapElementToUnifiedId(spawn->getElementType(), spawn->getElementDirection());

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
        std::cout << "✅ Mapa guardado exitosamente en YAML: " << filename.toStdString() << std::endl;
    } else {
        std::cout << "❌ Error al abrir archivo para escritura: " << file.errorString().toStdString() << std::endl;
    }
}

void MapEditor::processNewElement(MapElement* newElement) {
    connect(newElement, &MapElement::elementRemoved, this, &MapEditor::onElementRemoved, Qt::QueuedConnection);
    if (newElement->getElementType() == TYPE_HINT) {
        this->hintsInCurrentSegment.push_back(newElement);
        std::cout << "DEBUG: Hint agregado al segmento actual. Total: " << this->hintsInCurrentSegment.size() << std::endl;
    } else if (newElement->getElementType() == TYPE_START || newElement->getElementType() == TYPE_CHECKPOINT || newElement->getElementType() == TYPE_FINISH)
    {
        CircuitSegment newSegment;
        newSegment.cpElementPtr = newElement;

        if (!this->circuitSegments.empty())
            this->circuitSegments.back().segmentHints = this->hintsInCurrentSegment;

        this->circuitSegments.push_back(newSegment);
        this->hintsInCurrentSegment.clear();
        std::cout << "DEBUG: CP/Start/Finish colocado. Segmento cerrado. Nuevo Total: " << this->circuitSegments.size() << std::endl;
    } else if (newElement->getElementType() == TYPE_SPAWN) {
        this->mapSpawns.push_back(newElement);
        std::cout << "DEBUG: Spawn agregado. Total de spawns: " << this->mapSpawns.size() << std::endl;
    }
}

void MapEditor::onElementRemoved(MapElement* element) {
    std::cout << "DEBUG: Intentando eliminar Ptr: " << element << std::endl;

    if (element->getElementType() == TYPE_HINT) {
        auto& hints = this->hintsInCurrentSegment;

        hints.erase(std::remove_if(hints.begin(), hints.end(), [element](MapElement* ptr) { return ptr == element; }), hints.end());

        std::cout << "DEBUG: Hint removido de la lista ACTUAL. Total: " << hints.size() << std::endl;
    }

    for (auto& segment : this->circuitSegments) {
        auto& hints = segment.segmentHints;
        hints.erase(std::remove(hints.begin(), hints.end(), element), hints.end());
    }

    if (element->getElementType() != TYPE_HINT) {

        auto& segments = this->circuitSegments;

        segments.erase(std::remove_if(segments.begin(), segments.end(), [element](const CircuitSegment& s) { return s.cpElementPtr == element; }), segments.end());

        std::cout << "DEBUG: CP/Element removido. Nuevo total de segmentos: " << segments.size() << std::endl;
    }
}

MapElement* MapEditor::recreateElement(ElementType type, ElementDirection direction, double x, double y){
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

                if (!spawnNode["type_id"]) {
                    qWarning() << "Spawn sin type_id, se saltea.";
                    continue;
                }

                int typeId = spawnNode["type_id"].as<int>();
                ElementProperties props = mapUnifiedIdToTypeAndDirection(typeId);

                const YAML::Node& pos = spawnNode["position"];
                if (!pos || !pos.IsSequence() || pos.size() < 2) {
                    qWarning() << "Spawn con posición inválida.";
                    continue;
                }

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
            throw std::runtime_error("El nodo 'segments' no es una secuencia válida.");

        for (const auto& segmentNode : segmentsNode) {

            if (!segmentNode.IsMap()) continue;

            LoadedSegmentData segment;

            if (!segmentNode["segment_start_type"])
                throw std::runtime_error("Falta 'segment_start_type'.");

            int unifiedId = segmentNode["segment_start_type"].as<int>();
            ElementProperties cpProps = mapUnifiedIdToTypeAndDirection(unifiedId);

            segment.cpType = cpProps.type;
            segment.cpDirection = cpProps.direction;

            const YAML::Node& pos = segmentNode["position"];
            if (!pos || !pos.IsSequence() || pos.size() < 2) {
                qWarning() << "Segmento con posición inválida, se saltea.";
                continue;
            }

            segment.cpX = pos[0].as<double>();
            segment.cpY = pos[1].as<double>();

            const YAML::Node& hintsNode = segmentNode["hints_to_next_cp"];
            if (hintsNode && hintsNode.IsSequence()) {

                for (const auto& hintNode : hintsNode) {

                    if (!hintNode.IsMap()) continue;

                    if (!hintNode["type_id"]) {
                        qWarning() << "Hint sin type_id.";
                        continue;
                    }

                    int hintUnifiedId = hintNode["type_id"].as<int>();
                    ElementProperties hintProps = mapUnifiedIdToTypeAndDirection(hintUnifiedId);

                    const YAML::Node& hpos = hintNode["position"];
                    if (!hpos || !hpos.IsSequence() || hpos.size() < 2) {
                        qWarning() << "Hint con posición inválida.";
                        continue;
                    }

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
        QMessageBox::critical(this, "Error de YAML",
                              QString("No se pudo abrir el archivo: %1").arg(e.what()));
        return {};
    } catch (const YAML::Exception& e) {
        QMessageBox::critical(this, "Error de Parseo",
                              QString("Error al parsear YAML: %1").arg(e.what()));
        return {};
    } catch (const std::runtime_error& e) {
        QMessageBox::critical(this, "Error de estructura",
                              QString::fromStdString(e.what()));
        return {};
    }
    return result;
}

void MapEditor::setupLoadedCircuit(const LoadedMapData& loadedData){
    std::cout << "SLCT_DEBUG 1: Setup START. Segmentos cargados: "
              << loadedData.segments.size()
              << "segmento 1:"
              << (loadedData.segments.empty() ? 0 : loadedData.segments[0].hints.size())
              << ", Spawns: " << loadedData.spawns.size()
              << std::endl;

    this->circuitSegments.clear();
    this->hintsInCurrentSegment.clear();

    for (const auto& loadedSegment : loadedData.segments) {

        MapElement* cpElement = recreateElement(
            loadedSegment.cpType,
            loadedSegment.cpDirection,
            loadedSegment.cpX,
            loadedSegment.cpY
        );

        CircuitSegment newSegment;
        newSegment.cpElementPtr = cpElement;

        for (const auto& hintData : loadedSegment.hints) {
            std::cout << "RECREATING HINT at (" << hintData.x << ", " << hintData.y << ") dir=" << hintData.direction << std::endl;

            MapElement* hintElement = recreateElement(
                TYPE_HINT,
                hintData.direction,
                hintData.x,
                hintData.y
            );
            newSegment.segmentHints.push_back(hintElement);
        }

        this->circuitSegments.push_back(newSegment);
    }

    this->mapSpawns.clear();

    for (const auto& spawn : loadedData.spawns) {
        MapElement* spawnElement = recreateElement(
            TYPE_SPAWN,
            spawn.direction,
            spawn.x,
            spawn.y
        );

        this->mapSpawns.push_back(spawnElement);
    }

    if (!this->circuitSegments.empty()) {
        CircuitSegment& lastSegment = this->circuitSegments.back();

        if (lastSegment.cpElementPtr &&
            lastSegment.cpElementPtr->getElementType() == TYPE_FINISH)
        {
            QMessageBox::information(this, "Modo Edición",
                                     "Meta de llegada liberada para extensión del circuito.");

            MapElement* finishPtr = lastSegment.cpElementPtr;

            this->circuitSegments.pop_back();

            if (!this->circuitSegments.empty())
                this->hintsInCurrentSegment = this->circuitSegments.back().segmentHints;

            QObject::disconnect(finishPtr, nullptr, this, nullptr);
            if (finishPtr->scene()) finishPtr->scene()->removeItem(finishPtr);
            finishPtr->deleteLater();

            std::cout << "SLCT_DEBUG 6: Setup FINISHED (Meta Liberada)." << std::endl;
        }
        else {
            this->hintsInCurrentSegment = this->circuitSegments.back().segmentHints;
        }
    }

    std::cout << "SLCT_DEBUG FINAL: Circuit + Spawns reconstruidos correctamente." << std::endl;
}
