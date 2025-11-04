#include "mainmenu.h"
#include "ui_mainmenu.h"

#include <QFontDatabase>

MainMenu::MainMenu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainMenu)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setAutoFillBackground(true);

    QFontDatabase::addApplicationFont(":/media/orbitron.ttf");
}

void MainMenu::on_buttonNewGame_clicked()
{
    emit newGameClicked();
}

void MainMenu::on_buttonJoinGame_clicked()
{
    emit joinGameClicked();
}

void MainMenu::on_buttonExit_clicked()
{
    emit exitClicked();
}

MainMenu::~MainMenu()
{
    delete ui;
}
