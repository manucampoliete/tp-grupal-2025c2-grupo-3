#include "mapeditor.h"
#include "arrowitem.h"
#include <QVBoxLayout>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QPixmap>
#include <QGraphicsScene>
#include <iostream>

MapEditor::MapEditor(int cityId, QWidget* parent)
        : QWidget(parent),
        scene(new QGraphicsScene(this)),
        view(new MapView(scene, this))
{
    view->setScene(scene);

    QString mapPath  = ":/media/asset_vicecity.png";
    QString maskPath = ":/media/mask_vicecity.jpeg";

    QPixmap mapPixmap(mapPath);
    // Usamos std::cout en lugar de qDebug() para los errores de carga
    if (mapPixmap.isNull()) std::cout << "ERROR: No se pudo cargar el mapa" << std::endl;

            // mapItem DEBE SER UNA VARIABLE MIEMBRO de MapEditor
    mapItem = scene->addPixmap(mapPixmap);
    mapItem->setZValue(0);

            // --- Carga y escalado de collisionMask ---
    collisionMask.load(maskPath);

    if (collisionMask.isNull()) {
        std::cout << "ERROR CRÍTICO: La máscara NO SE PUDO CARGAR o es NULL." << std::endl;
        // Si falla la carga, detenemos la ejecución o usamos una máscara vacía
    }

            // 1. GARANTIZAR ESCALA DE GRISES (ANTES DE ESCALAR)
    collisionMask = collisionMask.convertToFormat(QImage::Format_Grayscale8);

            // DEBUG: Imprimir el tamaño original ANTES de escalar
    std::cout << "Máscara cargada ORIGINALMENTE: "
              << collisionMask.width() << "x" << collisionMask.height() << std::endl;

            // 2. ESCALADO ASEGURADO
    if (collisionMask.size() != mapPixmap.size()) {

        QSize targetSize = mapPixmap.size();

        // Asignamos el resultado de la función scaled() a la variable miembro
        collisionMask = collisionMask.scaled(
                targetSize, // Target: 4640x4672
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
                );

        // DEBUG: Imprimir el tamaño DESPUÉS de escalar para confirmar la corrección
        if (collisionMask.size() == targetSize) {
            std::cout << "ESCALADO EXITOSO. Nuevas dimensiones de la máscara: "
                      << collisionMask.width() << "x" << collisionMask.height() << std::endl;
        } else {
            std::cout << "ERROR CRÍTICO: Fallo al escalar la imagen. Dimensiones actuales: "
                      << collisionMask.width() << "x" << collisionMask.height() << std::endl;
        }

    }

            // --- LÓGICA DE PRUEBA: Añadir el ítem movible ---
    QPixmap arrowPixmap(":/media/arrow.png");

            // 3. ESCALAR LA FLECHA (Fix 1)
    const int desiredSize = 50;
    QPixmap scaledArrowPixmap = arrowPixmap.scaled(
            desiredSize, desiredSize,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );

            // La máscara que se pasa aquí ya tiene las dimensiones correctas (4640x4672)
    ArrowItem* arrow = new ArrowItem(scaledArrowPixmap, collisionMask);

    arrow->setPos(mapPixmap.width() / 2, mapPixmap.height() / 2);
    arrow->setFlag(QGraphicsItem::ItemIsMovable, true);
    arrow->setFlag(QGraphicsItem::ItemIsSelectable, true);
    arrow->setZValue(10);

    scene->addItem(arrow);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(view);
    setLayout(layout);

    view->fitInView(mapItem, Qt::KeepAspectRatio);
}
