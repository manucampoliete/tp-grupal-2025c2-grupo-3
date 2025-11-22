#include "toolbox.h"
#include <QDrag>
#include <QMimeData>
#include <QVBoxLayout>
#include <QLabel>

Toolbox::Toolbox(QWidget* parent) : QWidget(parent)
{
    // Solo para visualizar la flecha
    QLabel* arrowIcon = new QLabel(this);
    arrowIcon->setPixmap(QPixmap(":/media/arrow.png").scaled(40, 40));
    arrowIcon->setProperty("assetType", "arrow_hint"); // Propiedad para identificar el asset

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(arrowIcon, 0, Qt::AlignTop);
    layout->addStretch(); // Empuja el icono hacia arriba
    setLayout(layout);
}

void Toolbox::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {

        // 1. Identificar si el clic fue en un ícono arrastrable
        // (Simplificamos asumiendo que el clic en el ToolboxWidget siempre inicia el drag)

        QDrag* drag = new QDrag(this);
        QMimeData* mimeData = new QMimeData;

        // CRÍTICO: Usamos un tipo MIME simple para identificar qué estamos arrastrando
        mimeData->setText("asset/hint_arrow");

        // Opcional: Icono visual para el drag
        drag->setPixmap(QPixmap(":/media/arrow.png").scaled(40, 40));
        drag->setMimeData(mimeData);

        // Iniciar la operación de drag
        drag->exec(Qt::CopyAction);
    }
}
