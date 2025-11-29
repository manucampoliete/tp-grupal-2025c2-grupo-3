#include "joingame.h"
#include <QFontDatabase>
#include <vector>
#include "ui_joingame.h"

JoinGame::JoinGame(QWidget* parent, const std::vector<CarInfo>& cars):
        QWidget(parent), ui(new Ui::JoinGame), availableCars(cars) {
    ui->setupUi(this);

    errorLabel = new QLabel(this);
    errorLabel->setText("");
    errorLabel->setAlignment(Qt::AlignCenter);
    errorLabel->setStyleSheet("QLabel { color : red; }");
    ui->mainLayout->addWidget(errorLabel);

    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setAutoFillBackground(true);

    QFontDatabase::addApplicationFont(":/media/orbitron.ttf");

    carSelector = new CarSelector(this, availableCars);
    carSelector->setupCars(availableCars);
    ui->mainLayout->addWidget(carSelector);

    ui->buttonJoin->setFixedWidth(200);
}

void JoinGame::on_buttonReturn_clicked() { emit returnToMenuClicked(); }

void JoinGame::on_buttonJoin_clicked() {
    QString playerName = ui->lineEdit->text();
    QString gameId = ui->lineEdit_2->text();
    CarInfo selected = carSelector->getSelectedCar();

    emit joinGameRequested(playerName, gameId, selected);
}

void JoinGame::setJoinError() {
    errorLabel->setText("Invalid ID");
}

JoinGame::~JoinGame() { delete ui; }
