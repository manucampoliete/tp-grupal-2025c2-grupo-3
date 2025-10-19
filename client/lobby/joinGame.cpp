#include "joinGame.h"
#include "ui_joinGame.h"

join_game::join_game(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::join_game)
{
    ui->setupUi(this);
}

join_game::~join_game()
{
    delete ui;
}
