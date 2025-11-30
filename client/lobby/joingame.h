#ifndef JOINGAME_H
#define JOINGAME_H

#include <QWidget>
#include <vector>
#include <QLabel>
#include "carselector.h"

namespace Ui {
class JoinGame;
}

class JoinGame: public QWidget {
    Q_OBJECT

public:
    explicit JoinGame(QWidget* parent = nullptr, const std::vector<CarInfo>& cars = {});
    ~JoinGame();

    void setJoinError();

signals:
    void returnToMenuClicked();
    void joinGameRequested(const QString& username, const QString& gameId, const CarInfo& car);


private slots:
    void on_buttonReturn_clicked();
    void on_buttonJoin_clicked();


private:
    Ui::JoinGame* ui;
    CarSelector* carSelector;
    std::vector<CarInfo> availableCars;
    QLabel* errorLabel;
};

#endif  // JOINGAME_H
