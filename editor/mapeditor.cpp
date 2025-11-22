#include "mapeditor.h"
#include "arrowitem.h"
#include <QVBoxLayout>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QPixmap>
#include <QGraphicsScene>
#include <iostream>
#include "toolbox.h"

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
                targetSize,
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

    QPixmap arrowPixmap(":/media/arrow.png");

    const int desiredSize = 50;
    QPixmap scaledArrowPixmap = arrowPixmap.scaled(
            desiredSize, desiredSize,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );

    ArrowItem* arrow = new ArrowItem(scaledArrowPixmap, collisionMask);

    arrow->setPos(mapPixmap.width() / 2, mapPixmap.height() / 2);
    arrow->setFlag(QGraphicsItem::ItemIsMovable, true);
    arrow->setFlag(QGraphicsItem::ItemIsSelectable, true);
    arrow->setZValue(10);

    //scene->addItem(arrow);

    view->setMaskAndPixmap(collisionMask, scaledArrowPixmap);

    Toolbox* toolbox = new Toolbox(this);
    toolbox->setFixedWidth(150); // Darle un ancho fijo a la barra lateral

            // Usamos QHBoxLayout para poner la barra lateral y el mapa lado a lado
    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    // Conservamos el fix de márgenes y espaciado
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

            // Añadir la barra lateral a la izquierda o derecha
    mainLayout->addWidget(toolbox);

    // Añadir el MapView (el área principal del editor)
    mainLayout->addWidget(view);

    setLayout(mainLayout);

    view->fitInView(mapItem, Qt::KeepAspectRatio);

    const double initialZoomFactor = 30.0;
    view->scale(initialZoomFactor, initialZoomFactor);

    view->centerOn(mapItem);
}
