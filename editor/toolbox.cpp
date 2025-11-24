#include "toolbox.h"
#include "elementicon.h"
#include <QDrag>
#include <QMimeData>
#include <QVBoxLayout>
#include <QLabel>

Toolbox::Toolbox(QWidget* parent) : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop); // Alineación para que la lista empiece arriba

    // Título de la sección
    QLabel* title = new QLabel("Elementos de Carrera");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

            // --- SECCIÓN HINTS ---
    QLabel* hintLabel = new QLabel("Hints (12 Direcciones)");
    mainLayout->addWidget(hintLabel);

    QGridLayout* hintGrid = new QGridLayout;

    // ----------------------------------------------------
    // ORGANIZACIÓN DE LOS 12 HINTS EN UNA REJILLA 4x3
    // ----------------------------------------------------

            // Fila 0: UP y curvas superiores
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_UP,                this), 0, 1); // UP (0)
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_UP_LEFT,     this), 0, 0); // UL (4)
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_UP_RIGHT,    this), 0, 2); // UR (5)

    // Fila 1: Direcciones laterales
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_LEFT,              this), 1, 0); // LEFT (1)
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_RIGHT,             this), 1, 2); // RIGHT (2)

    // Fila 2: DOWN y curvas inferiores
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_DOWN,              this), 2, 1); // DOWN (3)
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_DOWN_LEFT,   this), 2, 0); // DL (6)
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_DOWN_RIGHT,  this), 2, 2); // DR (7)

            // Fila 3: Las 4 curvas restantes (secuencias de giro)
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_LEFT_UP,     this), 3, 0); // L-UP (8)
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_LEFT_DOWN,   this), 3, 1); // L-DOWN (9)
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_RIGHT_UP,    this), 3, 2); // R-UP (10)
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_RIGHT_DOWN,  this), 4, 1); // R-DOWN (11) - Nota: en la fila 4

    // Ajustar el espaciado
    hintGrid->setHorizontalSpacing(5);
    hintGrid->setVerticalSpacing(5);

    mainLayout->addLayout(hintGrid);

    QLabel* raceLabel = new QLabel("\nInicio y Meta"); // Añadir un salto de línea para separación
    mainLayout->addWidget(raceLabel);

    QGridLayout* raceGrid = new QGridLayout;

    // PUNTO DE PARTIDA (START)
    raceGrid->addWidget(new QLabel("Inicio:"), 0, 0);
    raceGrid->addWidget(new ElementIcon(TYPE_START, DIR_HORIZONTAL, this), 0, 1); // H
    raceGrid->addWidget(new ElementIcon(TYPE_START, DIR_VERTICAL,   this), 0, 2); // V

            // META DE LLEGADA (FINISH)
    raceGrid->addWidget(new QLabel("Meta:"), 1, 0);
    raceGrid->addWidget(new ElementIcon(TYPE_FINISH, DIR_HORIZONTAL, this), 1, 1); // H
    raceGrid->addWidget(new ElementIcon(TYPE_FINISH, DIR_VERTICAL,   this), 1, 2); // V

    mainLayout->addLayout(raceGrid);

    // --- ESPACIO PARA CHECKPOINTS/START/FINISH (futuro) ---
    mainLayout->addStretch();

    setLayout(mainLayout);
}
