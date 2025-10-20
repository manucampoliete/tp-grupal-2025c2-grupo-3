#ifndef JOINGAME_H
#define JOINGAME_H

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


private slots:
    void on_buttonReturn_clicked();


private:
    Ui::JoinGame *ui;
};

#endif // JOINGAME_H
