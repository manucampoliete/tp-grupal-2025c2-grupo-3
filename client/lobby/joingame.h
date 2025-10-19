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

private:
    Ui::JoinGame *ui;
};

#endif // JOINGAME_H
