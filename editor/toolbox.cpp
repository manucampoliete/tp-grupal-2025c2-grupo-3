#include "toolbox.h"
#include "elementicon.h"
#include <QDrag>
#include <QMimeData>
#include <QVBoxLayout>
#include <QLabel>
#include <qpushbutton.h>

Toolbox::Toolbox(QWidget* parent) : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);

    QLabel* title = new QLabel("Elementos de Carrera");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    QLabel* hintLabel = new QLabel("Hints (12 Direcciones)");
    mainLayout->addWidget(hintLabel);

    QGridLayout* hintGrid = new QGridLayout;

    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_UP,                this), 0, 1);
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_UP_LEFT,     this), 0, 0);
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_UP_RIGHT,    this), 0, 2);

    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_LEFT,              this), 1, 0);
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_RIGHT,             this), 1, 2);

    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_DOWN,              this), 2, 1);
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_DOWN_LEFT,   this), 2, 0);
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_DOWN_RIGHT,  this), 2, 2);

    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_LEFT_UP,     this), 3, 0);
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_LEFT_DOWN,   this), 3, 1);
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_RIGHT_UP,    this), 3, 2);
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_RIGHT_DOWN,  this), 4, 1);

    hintGrid->setHorizontalSpacing(5);
    hintGrid->setVerticalSpacing(5);

    mainLayout->addLayout(hintGrid);

    QLabel* raceLabel = new QLabel("\nInicio y Meta");
    mainLayout->addWidget(raceLabel);

    QGridLayout* raceGrid = new QGridLayout;

    raceGrid->addWidget(new QLabel("Inicio:"), 0, 0);
    raceGrid->addWidget(new ElementIcon(TYPE_START, DIR_HORIZONTAL, this), 0, 1);
    raceGrid->addWidget(new ElementIcon(TYPE_START, DIR_VERTICAL,   this), 0, 2);

    raceGrid->addWidget(new QLabel("Meta:"), 1, 0);
    raceGrid->addWidget(new ElementIcon(TYPE_FINISH, DIR_HORIZONTAL, this), 1, 1);
    raceGrid->addWidget(new ElementIcon(TYPE_FINISH, DIR_VERTICAL,   this), 1, 2);

    mainLayout->addLayout(raceGrid);

    mainLayout->addStretch();

    QPushButton* saveButton = new QPushButton("Guardar Mapa");

    saveButton->setStyleSheet("background-color: #007bff; color: white; padding: 10px; border-radius: 5px;");

    mainLayout->addWidget(saveButton);

    connect(saveButton, &QPushButton::clicked, this, &Toolbox::saveClicked);

    setLayout(mainLayout);

    mainLayout->addStretch();

    setLayout(mainLayout);
}
