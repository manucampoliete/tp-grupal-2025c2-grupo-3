#include "newgame.h"

#include <QFontDatabase>
#include <iostream>

#include "../../common/utils/carinfo.h"

#include "ui_newgame.h"

NewGame::NewGame(QWidget* parent, const std::vector<CarInfo>& cars):
        QWidget(parent), ui(new Ui::NewGame), available_cars(cars) {
    ui->setupUi(this);

    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setAutoFillBackground(true);

    QFontDatabase::addApplicationFont(":/media/orbitron.ttf");

    carSelector = new CarSelector(this, available_cars);
    carSelector->setupCars(available_cars);
    ui->mainLayout->addWidget(carSelector);

    ui->buttonCreate->setFixedWidth(350);
    ui->buttonReturn->setFixedWidth(150);
}

void NewGame::on_buttonReturn_clicked() { emit returnToMenuClicked(); }

void NewGame::on_buttonCreate_clicked() {
    QString player_name = ui->lineEdit->text();
    CarInfo selected = carSelector->getSelectedCar();

    emit newGameRequested(player_name, selected);
}

NewGame::~NewGame() { delete ui; }
