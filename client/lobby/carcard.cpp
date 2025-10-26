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

    QLabel *healthTitleLabel = new QLabel("Salud   ", this);
    QProgressBar *healthBar = new QProgressBar(this);

    QLabel *speedTitleLabel = new QLabel("Velocidad   ", this);
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

    // Código dentro del constructor de CarCard o CarSelector
    this->setStyleSheet(
        "QProgressBar {"
        /* Estilo del Contenedor (parte vacía de la barra) */
        "background-color: #2e2e42;"
        "border: 1px solid #555577;"
        "border-radius: 4px;"
        "min-height: 12px;"
        "}"

        "QProgressBar::chunk {"
        /* Estilo de la Porción Llena (el progreso) */
        /* Color Verde Futuro/Brillante */
        "background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, "
        "stop: 0 #00BFFF, stop: 1 #00FFFF); "
        "border-radius: 4px;"
        "}"

        // --- Estilo de Botón (General) ---
        "QPushButton {"
        "background-color: #00BFFF;"
        "color: #a3be8c;"
        "font-size: 15pt;"
        "font-weight: bold;"
        "border: none;"
        "border-radius: 3px;"
        "padding: 15px 10px 15px 10px;"

        /* Asegura que el estilo general del botón no sea transparente si lo es por defecto */
        "opacity: 1.0;"
        "}"

        // --- Estilo de Botón (Hover: Anulación para eliminar el efecto nativo) ---
        "QPushButton:hover {"
        /* Mantener el mismo color de fondo y color de texto para anular el hover nativo */
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

