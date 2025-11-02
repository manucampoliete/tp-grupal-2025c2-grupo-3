#include "carcard.h"

#include <QProgressBar>
#include <QVBoxLayout>
#include <iostream>

CarCard::CarCard(const CarInfo &car, QWidget *parent)
    : QWidget(parent)
{
    this->setMaximumSize(600, 200);

    imageLabel = new QLabel(this);
    nameLabel = new QLabel(this);

    QLabel *healthTitleLabel = new QLabel("Health", this);
    QProgressBar *healthBar = new QProgressBar(this);

    QLabel *speedTitleLabel = new QLabel("Speed", this);
    QProgressBar *speedBar = new QProgressBar(this);

    healthTitleLabel->setFixedWidth(140);
    speedTitleLabel->setFixedWidth(140);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setHorizontalSpacing(20);

    healthBar->setRange(0, 100);
    healthBar->setValue(car.health);
    healthBar->setTextVisible(false);

    speedBar->setRange(0, 10);
    speedBar->setValue(car.speed);
    speedBar->setTextVisible(false);

    QHBoxLayout *healthLayout = new QHBoxLayout();
    healthLayout->addWidget(healthTitleLabel);
    healthLayout->addWidget(healthBar);
    healthLayout->addStretch(1);

    QHBoxLayout *speedLayout = new QHBoxLayout();
    speedLayout->addWidget(speedTitleLabel);
    speedLayout->addWidget(speedBar);
    speedLayout->addStretch(1);

    gridLayout->addWidget(imageLabel, 0, 0, 3, 1);

    nameLabel->setText(QString::fromStdString(car.name));
    nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->addWidget(nameLabel);
    nameLayout->addStretch(1);
    gridLayout->addLayout(nameLayout, 0, 1);

    gridLayout->addLayout(healthLayout, 1, 1);

    gridLayout->addLayout(speedLayout, 2, 1);

    this->setLayout(gridLayout);

    this->setStyleSheet(
        "QProgressBar {"
        "background-color: #2e2e42;"
        "border: 1px solid #555577;"
        "min-height: 12px;"
        "}"

        "QProgressBar::chunk {"
        "background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, "
        "stop: 0 #0099FF, stop: 1 #00CCFF);"
        "}"

        "QPushButton {"
        "background-color: #00BFFF;"
        "color: #a3be8c;"
        "font-size: 15pt;"
        "font-weight: bold;"
        "border: none;"
        "border-radius: 3px;"
        "padding: 15px 10px 15px 10px;"
        "opacity: 1.0;"
        "}"

        "QPushButton:hover {"
        "background-color: #00BFFF;"
        "color: #a3be8c;"
        "border: none;"
        "}"
        );

    gridLayout->setColumnStretch(0, 20);
    gridLayout->setColumnStretch(1, 80);
    gridLayout->setRowStretch(0, 1);
    gridLayout->setRowStretch(1, 1);
    gridLayout->setRowStretch(2, 1);

    QString imagePath = QString(":/media/cars/%1.png").arg(car.id);
    QPixmap carImage(imagePath);

    if (!carImage.isNull()) {
        QSize desiredSize(400, 200);

        imageLabel->setPixmap(
            carImage.scaled(
                desiredSize,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                )
            );
    } else {
        imageLabel->setText("Image Not Found: " + imagePath);
        std::cout << "Image not found at path: " << imagePath.toStdString() << std::endl;
    }
}

CarCard::~CarCard()
{
}

