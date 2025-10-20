#include "joingame.h"
#include "ui_joingame.h"

#include <QFontDatabase>

JoinGame::JoinGame(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::JoinGame)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setAutoFillBackground(true);

    QFontDatabase::addApplicationFont(":/media/orbitron.ttf");
}

void JoinGame::on_buttonReturn_clicked()
{
    emit returnToMenuClicked();
}

JoinGame::~JoinGame()
{
    delete ui;
}
