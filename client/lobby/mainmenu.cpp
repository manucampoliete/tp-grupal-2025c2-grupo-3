#include "mainmenu.h"
#include "ui_mainmenu.h"

MainMenu::MainMenu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainMenu)
{
    ui->setupUi(this);

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
