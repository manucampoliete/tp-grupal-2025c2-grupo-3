#include "newgame.h"
#include "ui_newgame.h"

#include <QFontDatabase>

NewGame::NewGame(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NewGame)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setAutoFillBackground(true);

    QFontDatabase::addApplicationFont(":/media/orbitron.ttf");
}

void NewGame::on_buttonReturn_clicked()
{
    emit returnToMenuClicked();
}

NewGame::~NewGame()
{
    delete ui;
}
