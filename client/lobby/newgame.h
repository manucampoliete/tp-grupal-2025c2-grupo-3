#ifndef NEWGAME_H
#define NEWGAME_H

#include <QWidget>
#include <vector>
#include "carselector.h"

namespace Ui {
class NewGame;
}

class NewGame: public QWidget {
    Q_OBJECT

public:
    explicit NewGame(QWidget* parent = nullptr, const std::vector<CarInfo>& cars = {});
    ~NewGame();

signals:
    void returnToMenuClicked();
    void newGameRequested(const QString& username, const CarInfo& car);


private slots:
    void on_buttonReturn_clicked();
    void on_buttonCreate_clicked();

private:
    Ui::NewGame* ui;
    CarSelector* carSelector;
    std::vector<CarInfo> availableCars;
};

#endif  // NEWGAME_H
