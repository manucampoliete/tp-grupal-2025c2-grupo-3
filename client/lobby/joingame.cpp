#include "joingame.h"

#include <QFontDatabase>
#include <vector>

#include "ui_joingame.h"

JoinGame::JoinGame(QWidget* parent, const std::vector<CarInfo>& cars):
        QWidget(parent), ui(new Ui::JoinGame), available_cars(cars) {
    ui->setupUi(this);

    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setAutoFillBackground(true);

    QFontDatabase::addApplicationFont(":/media/orbitron.ttf");

    carSelector = new CarSelector(this);
    carSelector->setupCars(available_cars);
    ui->mainLayout->addWidget(carSelector);

    ui->buttonJoin->setFixedWidth(200);
}

void JoinGame::on_buttonReturn_clicked() { emit returnToMenuClicked(); }

void JoinGame::on_buttonJoin_clicked() {
    QString player_name = ui->lineEdit->text();
    QString game_id = ui->lineEdit_2->text();
    CarInfo selected = carSelector->getSelectedCar();

    emit joinGameRequested(player_name, game_id, selected);
}

JoinGame::~JoinGame() { delete ui; }
