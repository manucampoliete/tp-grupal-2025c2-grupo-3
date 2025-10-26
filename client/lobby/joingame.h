#ifndef JOINGAME_H
#define JOINGAME_H

#include "carselector.h"

#include <QWidget>

namespace Ui {
class JoinGame;
}

class JoinGame : public QWidget
{
    Q_OBJECT

public:
    explicit JoinGame(QWidget *parent = nullptr);
    ~JoinGame();

signals:
    void returnToMenuClicked();
    void joinGameRequested();
    //void startLobby(const QString &carId, const QString &ip, const QString &port);


private slots:
    void on_buttonReturn_clicked();
    void on_buttonJoin_clicked();


private:
    Ui::JoinGame *ui;
    CarSelector *carSelector;
};

#endif // JOINGAME_H
