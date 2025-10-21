#include "carselector.h"
#include "carcard.h"
#include "ui_carselector.h"

#include <QHBoxLayout>

CarSelector::CarSelector(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CarSelector)
{
    ui->setupUi(this);

    //carStack = new QStackedWidget(this);
    //prevButton = new QPushButton("<-", this);
    //nextButton = new QPushButton("->", this);

    // 1. Conexiones de navegación
    connect(ui->prevButton, &QPushButton::clicked, this, &CarSelector::on_prevButton_clicked);
    connect(ui->nextButton, &QPushButton::clicked, this, &CarSelector::on_nextButton_clicked);

    // 2. Crear las tarjetas (debes implementar setupCars)
    setupCars();

    // 3. Organizar el diseño de CarSelector (Carrusel y Flechas)
    //QHBoxLayout *navLayout = new QHBoxLayout();
    //navLayout->addWidget(prevButton);
    //navLayout->addWidget(carStack, 1);
    //navLayout->addWidget(nextButton);

    //this->setLayout(navLayout);
}

void CarSelector::setupCars()
{
    ui->carStack->addWidget(new CarCard("MAZDA_RX7", this));
    ui->carStack->addWidget(new CarCard("HONDA_S2000", this));
    ui->carStack->addWidget(new CarCard("NISSAN_GTR", this));
}

void CarSelector::on_prevButton_clicked()
{
    int currentIndex = ui->carStack->currentIndex();
    int maxIndex = ui->carStack->count() - 1;

    if (currentIndex > 0) {
        ui->carStack->setCurrentIndex(currentIndex - 1);
    } else {
        // Opción: Ir al final si está en el primero
        ui->carStack->setCurrentIndex(maxIndex);
    }
}

void CarSelector::on_nextButton_clicked()
{
    int currentIndex = ui->carStack->currentIndex();
    int maxIndex = ui->carStack->count() - 1;

    if (currentIndex < maxIndex) {
        ui->carStack->setCurrentIndex(currentIndex + 1);
    } else {
        // Opción: Volver al inicio si está en el último
        ui->carStack->setCurrentIndex(0);
    }
}

CarSelector::~CarSelector()
{
    delete ui;
}
