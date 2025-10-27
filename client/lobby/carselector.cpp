#include "carselector.h"
#include "carcard.h"
//#include "ui_carselector.h"

#include <QHBoxLayout>

CarSelector::CarSelector(QWidget *parent)
    : QWidget(parent)
{
    this->setObjectName("CarSelector");

    carStack = new QStackedWidget(this);
    prevButton = new QPushButton("◀", this);
    nextButton = new QPushButton("▶", this);

    connect(prevButton, &QPushButton::clicked, this, &CarSelector::on_prevButton_clicked);
    connect(nextButton, &QPushButton::clicked, this, &CarSelector::on_nextButton_clicked);

    setupCars();

    QHBoxLayout *navLayout = new QHBoxLayout();
    navLayout->addWidget(prevButton);
    navLayout->addWidget(carStack, 1);
    navLayout->addWidget(nextButton);

    this->setLayout(navLayout);

    int topMargin = 30;
    int generalMargin = 5;

    this->layout()->setContentsMargins(
        generalMargin, // Margen izquierdo
        topMargin,     // Margen SUPERIOR
        generalMargin, // Margen inferior
        generalMargin  // Margen derecho
        );
}

void CarSelector::setupCars()
{
    carStack->addWidget(new CarCard("MAZDA", this));
    carStack->addWidget(new CarCard("HONDA", this));
    carStack->addWidget(new CarCard("NISSAN", this));
}

void CarSelector::on_prevButton_clicked()
{
    int currentIndex = carStack->currentIndex();
    int maxIndex = carStack->count() - 1;

    if (currentIndex > 0) {
        carStack->setCurrentIndex(currentIndex - 1);
    } else {
        carStack->setCurrentIndex(maxIndex);
    }
}

void CarSelector::on_nextButton_clicked()
{
    int currentIndex = carStack->currentIndex();
    int maxIndex = carStack->count() - 1;

    if (currentIndex < maxIndex) {
        carStack->setCurrentIndex(currentIndex + 1);
    } else {
        carStack->setCurrentIndex(0);
    }
}

CarSelector::~CarSelector()
{
}
