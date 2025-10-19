#include "newGame.h"
#include "ui_newGame.h"

new_game::new_game(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::new_game)
{
    ui->setupUi(this);
}

new_game::~new_game()
{
    delete ui;
}
