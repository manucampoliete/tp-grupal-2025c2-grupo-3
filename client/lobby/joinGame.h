#ifndef JOINGAME_H
#define JOINGAME_H

#include <QWidget>

namespace Ui {
class join_game;
}

class join_game : public QWidget
{
    Q_OBJECT

public:
    explicit join_game(QWidget *parent = nullptr);
    ~join_game();

private:
    Ui::join_game *ui;
};

#endif // JOINGAME_H
