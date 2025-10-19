#ifndef NEWGAME_H
#define NEWGAME_H

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

private:
    Ui::NewGame *ui;
};

#endif // NEWGAME_H
