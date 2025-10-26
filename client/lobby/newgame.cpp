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

    carSelector = new CarSelector(this);
    ui->mainLayout->addWidget(carSelector);
}

void NewGame::on_buttonReturn_clicked()
{
    emit returnToMenuClicked();
}

void NewGame::on_buttonCreate_clicked()
{
    emit newGameRequested();
}

NewGame::~NewGame()
{
    delete ui;
}
