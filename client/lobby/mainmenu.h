#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>

namespace Ui {
class MainMenu;
}

class MainMenu : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenu(QWidget *parent = nullptr);
    ~MainMenu();

signals:
    void newGameClicked();
    void joinGameClicked();
    void exitClicked();

private slots:
    void on_buttonNewGame_clicked();
    void on_buttonJoinGame_clicked();
    void on_buttonExit_clicked();

private:
    Ui::MainMenu *ui;
};

#endif // MAINMENU_H
