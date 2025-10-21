#include "carcard.h"
#include "ui_carcard.h"

#include <QVBoxLayout>

CarCard::CarCard(const QString &carId, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CarCard), m_carId(carId)
{
    ui->setupUi(this);
    ui->nameLabel->setText(m_carId);
    ui->nameLabel->setAlignment(Qt::AlignCenter);

    //carId.toLower()
    QString imagePath = ":/media/car.jpg";
    QPixmap carImage(imagePath);

    if (!carImage.isNull()) {
        QSize desiredSize(400, 200);

        ui->imageLabel->setPixmap(
            carImage.scaled(
                desiredSize,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                )
            );

        ui->imageLabel->setMinimumSize(desiredSize);

    } else {
        ui->imageLabel->setText("Image Not Found: " + imagePath);
    }
}

CarCard::~CarCard()
{
    delete ui;
}

