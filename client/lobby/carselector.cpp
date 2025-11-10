#include "carselector.h"
#include "carcard.h"
//#include "ui_carselector.h"

#include <QHBoxLayout>
#include <iostream>

CarSelector::CarSelector(QWidget *parent, const std::vector<CarInfo>& cars)
    : QWidget(parent), car_list(cars)
{
    this->setObjectName("CarSelector");

    carStack = new QStackedWidget(this);
    prevButton = new QPushButton("◀", this);
    nextButton = new QPushButton("▶", this);

    connect(prevButton, &QPushButton::clicked, this, &CarSelector::on_prevButton_clicked);
    connect(nextButton, &QPushButton::clicked, this, &CarSelector::on_nextButton_clicked);

    QHBoxLayout *navLayout = new QHBoxLayout();
    navLayout->addWidget(prevButton);
    navLayout->addWidget(carStack, 1);
    navLayout->addWidget(nextButton);

    this->setLayout(navLayout);

    int topMargin = 30;
    int generalMargin = 5;

    this->layout()->setContentsMargins(
        generalMargin, // izquierdo
        topMargin,     // superior
        generalMargin, // inferior
        generalMargin  // derecho
        );
}

void CarSelector::setupCars(const std::vector<CarInfo>& cars)
{
    for (const auto& car : cars) 
        carStack->addWidget(new CarCard(car, this));
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

CarInfo CarSelector::getSelectedCar() const
{
    int index = carStack->currentIndex();
    std::cerr << "index: " << index << std::endl;
    if (index >= 0 && index < static_cast<int>(car_list.size()))
        return car_list[index];

    return {0, "Unknown", 0, 0};
}

CarSelector::~CarSelector()
{
}
