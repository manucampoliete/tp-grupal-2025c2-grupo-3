#include "toolbox.h"
#include "elementicon.h"
#include <QDrag>
#include <QMimeData>
#include <QVBoxLayout>
#include <QLabel>
#include <qpushbutton.h>

Toolbox::Toolbox(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);

    QLabel* title = new QLabel("Elements");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    QLabel* hintLabel = new QLabel("Hints");
    mainLayout->addWidget(hintLabel);

    QGridLayout* hintGrid = new QGridLayout;

    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_UP, this), 0, 1);
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_UP_LEFT, this), 0, 0);
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_UP_RIGHT, this), 0, 2);

    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_LEFT, this), 1, 0);
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_RIGHT, this), 1, 2);
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_RIGHT_DOWN, this), 1, 1);

    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_DOWN, this), 2, 1);
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_DOWN_LEFT, this), 2, 0);
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_DOWN_RIGHT, this), 2, 2);

    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_LEFT_UP, this), 3, 0);
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_LEFT_DOWN, this), 3, 1);
    hintGrid->addWidget(new ElementIcon(TYPE_HINT, DIR_CURVE_RIGHT_UP, this), 3, 2);
    

    hintGrid->setHorizontalSpacing(5);
    hintGrid->setVerticalSpacing(5);

    mainLayout->addLayout(hintGrid);

    QLabel* spawnLabel = new QLabel("\nSpawn Boxes");
    mainLayout->addWidget(spawnLabel);

    QGridLayout* spawnGrid = new QGridLayout;

    spawnGrid->addWidget(new ElementIcon(TYPE_SPAWN, DIR_UP, this), 0, 1);

    spawnGrid->addWidget(new ElementIcon(TYPE_SPAWN, DIR_LEFT, this), 1, 0);
    spawnGrid->addWidget(new ElementIcon(TYPE_SPAWN, DIR_RIGHT, this), 1, 2);

    spawnGrid->addWidget(new ElementIcon(TYPE_SPAWN, DIR_DOWN, this), 2, 1);

    spawnGrid->setHorizontalSpacing(5);
    spawnGrid->setVerticalSpacing(5);

    mainLayout->addLayout(spawnGrid);

    QGridLayout* raceGrid = new QGridLayout;

    raceGrid->addWidget(new QLabel("Start:"), 0, 0);
    raceGrid->addWidget(new ElementIcon(TYPE_START, DIR_HORIZONTAL, this), 0, 1);
    raceGrid->addWidget(new ElementIcon(TYPE_START, DIR_VERTICAL,   this), 0, 2);

    raceGrid->addWidget(new QLabel("Finish:"), 1, 0);
    raceGrid->addWidget(new ElementIcon(TYPE_FINISH, DIR_HORIZONTAL, this), 1, 1);
    raceGrid->addWidget(new ElementIcon(TYPE_FINISH, DIR_VERTICAL,   this), 1, 2);

    mainLayout->addLayout(raceGrid);

    QLabel* cpLabel = new QLabel("\nCheckpoints");
    mainLayout->addWidget(cpLabel);

    QGridLayout* cpGrid = new QGridLayout;
    cpGrid->addWidget(new ElementIcon(TYPE_CHECKPOINT, DIR_HORIZONTAL, this), 0, 1);
    cpGrid->addWidget(new ElementIcon(TYPE_CHECKPOINT, DIR_VERTICAL,   this), 0, 0);

    mainLayout->addLayout(cpGrid);
    
    QPushButton* saveButton = new QPushButton("Save Map");
    
    saveButton->setStyleSheet("background-color: #050505; color: white; padding: 10px; border-radius: 5px;");
    mainLayout->addSpacing(25);

    mainLayout->addWidget(saveButton);

    connect(saveButton, &QPushButton::clicked, this, &Toolbox::saveClicked);

    setLayout(mainLayout);
}
