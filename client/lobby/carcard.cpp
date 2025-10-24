#include "carcard.h"

#include <QProgressBar>
#include <QVBoxLayout>

CarCard::CarCard(const QString &carId, QWidget *parent)
    : QWidget(parent)
    , m_carId(carId)
{
    this->setMaximumSize(600, 200);

    imageLabel = new QLabel(this);
    nameLabel = new QLabel(this);

    QLabel *healthTitleLabel = new QLabel("Salud:", this);
    QProgressBar *healthBar = new QProgressBar(this);

    QLabel *speedTitleLabel = new QLabel("Velocidad:", this);
    QProgressBar *speedBar = new QProgressBar(this);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setHorizontalSpacing(20);

    healthBar->setRange(0, 100);
    healthBar->setValue(60); // Ejemplo: 60% de salud
    healthBar->setTextVisible(false);

    speedBar->setRange(0, 10);
    speedBar->setValue(8); // Ejemplo: 8 de 10 de velocidad
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

    nameLabel->setText(m_carId);
    nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->addWidget(nameLabel);
    nameLayout->addStretch(1);
    gridLayout->addLayout(nameLayout, 0, 1);

    gridLayout->addLayout(healthLayout, 1, 1);

    gridLayout->addLayout(speedLayout, 2, 1);

    this->setLayout(gridLayout);

    gridLayout->setColumnStretch(0, 20);
    gridLayout->setColumnStretch(1, 80);
    gridLayout->setRowStretch(0, 1);
    gridLayout->setRowStretch(1, 1);
    gridLayout->setRowStretch(2, 1);

    QString imagePath = ":/media/car.jpg";
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
        //imageLabel->setMinimumSize(desiredSize);
    } else {
        imageLabel->setText("Image Not Found: " + imagePath);
    }
}

CarCard::~CarCard()
{
}

