#ifndef NEWGAME_H
#define NEWGAME_H

#include "carselector.h"

#include <QWidget>

namespace Ui {
class NewGame;
}

class NewGame : public QWidget
{
    Q_OBJECT

public:
    explicit NewGame(QWidget *parent = nullptr);
    ~NewGame();

signals:
    void returnToMenuClicked();
    void newGameRequested();


private slots:
    void on_buttonReturn_clicked();
    void on_buttonCreate_clicked();

private:
    Ui::NewGame *ui;
    CarSelector *carSelector;
};

#endif // NEWGAME_H
