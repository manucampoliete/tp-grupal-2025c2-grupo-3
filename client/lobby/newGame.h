#ifndef NEWGAME_H
#define NEWGAME_H

#include <QWidget>

namespace Ui {
class new_game;
}

class new_game : public QWidget
{
    Q_OBJECT

public:
    explicit new_game(QWidget *parent = nullptr);
    ~new_game();

private:
    Ui::new_game *ui;
};

#endif // NEWGAME_H
