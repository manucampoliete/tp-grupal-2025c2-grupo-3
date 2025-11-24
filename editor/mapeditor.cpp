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


MapEditor::MapEditor(int cityId, QWidget* parent)
        : QWidget(parent),
        scene(new QGraphicsScene(this)),
        view(new MapView(scene, this))
{
    this->setMinimumSize(1280, 720);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setScene(scene);

    QString mapPath  = ":/media/asset_vicecity.png";
    QString maskPath = ":/media/mask_vicecity.jpeg";

    QPixmap mapPixmap(mapPath);
    if (mapPixmap.isNull()) std::cout << "ERROR: No se pudo cargar el mapa" << std::endl;

    mapItem = scene->addPixmap(mapPixmap);
    mapItem->setZValue(0);

    collisionMask.load(maskPath);

    if (collisionMask.isNull()) {
        std::cout << "ERROR CRÍTICO: La máscara NO SE PUDO CARGAR o es NULL." << std::endl;
    }

    collisionMask = collisionMask.convertToFormat(QImage::Format_Grayscale8);

    std::cout << "Máscara cargada ORIGINALMENTE: "
              << collisionMask.width() << "x" << collisionMask.height() << std::endl;

    if (collisionMask.size() != mapPixmap.size()) {

        QSize targetSize = mapPixmap.size();

        collisionMask = collisionMask.scaled(
                targetSize, // Target: 4640x4672
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
                );

        if (collisionMask.size() == targetSize) {
            std::cout << "ESCALADO EXITOSO. Nuevas dimensiones de la máscara: "
                      << collisionMask.width() << "x" << collisionMask.height() << std::endl;
        } else {
            std::cout << "ERROR CRÍTICO: Fallo al escalar la imagen. Dimensiones actuales: "
                      << collisionMask.width() << "x" << collisionMask.height() << std::endl;
        }
    }

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

    view->fitInView(mapItem, Qt::KeepAspectRatio);

    const double initialZoomFactor = 30.0;
    view->scale(initialZoomFactor, initialZoomFactor);

    view->centerOn(mapItem);
}

void MapEditor::serializeToYaml(const QString& filename)
{
    std::vector<CircuitSegment>& circuit = this->circuitSegments;

    if (circuit.empty()) {
        std::cout << "Advertencia: No hay checkpoints o puntos de inicio/fin para guardar." << std::endl;
        return;
    }

    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "map_name" << YAML::Value << "City_Map_1";

    emitter << YAML::Key << "segments";
    emitter << YAML::Value << YAML::BeginSeq;

    for (const auto& segment : circuit) {
        emitter << YAML::BeginMap;

        emitter << YAML::Key << "segment_start_type" << YAML::Value << typeToString(segment.cpType).toStdString();
        emitter << YAML::Key << "position";
        emitter << YAML::Value << YAML::BeginSeq << segment.cpPosition.x() << segment.cpPosition.y() << YAML::EndSeq;

        emitter << YAML::Key << "direction" << YAML::Value << directionToString(segment.cpDirection).toStdString();

        emitter << YAML::Key << "hints_to_next_cp";
        emitter << YAML::Value << YAML::BeginSeq;

        for (const auto& hint : segment.segmentHints) {
            emitter << YAML::BeginMap;

            emitter << YAML::Key << "type_id" << YAML::Value << hint.direction;
            emitter << YAML::Key << "position";
            emitter << YAML::Value << YAML::BeginSeq << hint.position.x() << hint.position.y() << YAML::EndSeq;

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

void MapEditor::onSaveRequest()
{
    QString filter = "Archivos de Mapa YAML (*.yaml *.yml)";
    QString defaultPath = QDir::homePath() + "/nuevo_mapa.yaml";

    QString fileName = QFileDialog::getSaveFileName(
            this,
            "Guardar Circuito de Carrera",
            defaultPath,
            filter
            );

    if (!fileName.isEmpty()) {
        if (!fileName.toLower().endsWith(".yaml") && !fileName.toLower().endsWith(".yml")) {
            fileName += ".yaml";
        }

        std::cout << "Iniciando serialización a: " << fileName.toStdString() << std::endl;

        serializeToYaml(fileName);

    } else {
        std::cout << "Guardado cancelado por el usuario." << std::endl;
    }
}

void MapEditor::processNewElement(MapElement* newElement) {
    if (newElement->getElementType() == TYPE_HINT) {
        HintData hint;
        hint.direction = newElement->getElementDirection();
        hint.position = newElement->pos();
        hintsInCurrentSegment.push_back(hint);
        std::cout << "DEBUG: Hint agregado al segmento actual. Total: " << hintsInCurrentSegment.size() << std::endl;

    } else if (newElement->getElementType() == TYPE_START ||
               newElement->getElementType() == TYPE_CHECKPOINT ||
               newElement->getElementType() == TYPE_FINISH)
    {
        CircuitSegment newSegment;
        newSegment.cpType = newElement->getElementType();
        newSegment.cpDirection = newElement->getElementDirection();
        newSegment.cpPosition = newElement->pos();

        if (!circuitSegments.empty()) {
            circuitSegments.back().segmentHints = hintsInCurrentSegment;
        }

        circuitSegments.push_back(newSegment);

        hintsInCurrentSegment.clear();
        std::cout << "DEBUG: CP/Start/Finish colocado. Segmento cerrado. Nuevo Total: " << circuitSegments.size() << std::endl;
    }
}
