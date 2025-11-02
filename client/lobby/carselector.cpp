#include "carselector.h"
#include "carcard.h"
//#include "ui_carselector.h"

#include <QHBoxLayout>
#include <iostream>

CarSelector::CarSelector(QWidget *parent)
    : QWidget(parent)
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
    /* car_list.clear();
    carStack->clear();

    car_list = cars; */

    car_list = {
        {0, "Jeep Wrangler", 80, 6},
        {1, "Ferrari F40", 90, 9},
        {2, "BMW Z4", 85, 8},
        {3, "VW Beetle", 75, 7},
        {4, "Ford Bronco", 88, 7},
        {5, "Ford F100", 95, 10},
        {6, "MB S-Class", 92, 8}
    };

    for (const auto& car : car_list) 
        carStack->addWidget(new CarCard(car, this));

    for (const auto& car : cars) 
        std::cout << "Auto recibido: " << car.name << std::endl;
    
    /* CarInfo jeep = {0, "Jeep Wrangler", 80, 6};
    CarInfo ferrari = {1, "Ferrari F40", 90, 9};
    CarInfo bmw = {2, "BMW Z4", 85, 8};
    CarInfo vw = {3, "VW Beetle", 75, 7};
    CarInfo bronco = {4, "Ford Bronco", 88, 7};
    CarInfo f100 = {5, "Ford F100", 95, 10};
    CarInfo merc = {6, "MB S-Class", 92, 8};

    carStack->addWidget(new CarCard(jeep, this));
    carStack->addWidget(new CarCard(ferrari, this));
    carStack->addWidget(new CarCard(bmw, this));
    carStack->addWidget(new CarCard(vw, this));
    carStack->addWidget(new CarCard(bronco, this));
    carStack->addWidget(new CarCard(f100, this));
    carStack->addWidget(new CarCard(merc, this));  */
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
    if (index >= 0 && index < static_cast<int>(car_list.size()))
        return car_list[index];

    return {-1, "Unknown", 0, 0};
}

CarSelector::~CarSelector()
{
}
