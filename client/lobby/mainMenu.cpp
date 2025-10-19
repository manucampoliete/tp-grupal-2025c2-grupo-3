#include "mainMenu.h"
#include "ui_mainMenu.h"

main_menu::mainMenu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::mainMenu)
{
    ui->setupUi(this);
}

main_menu::~mmainMenu()
{
    delete ui;
}
