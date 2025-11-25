#ifndef MENU_H
#define MENU_H

#include <QWidget>
#include "mapoption.h"

namespace Ui {
class Menu;
}

class Menu: public QWidget {
    Q_OBJECT

public:
    explicit Menu(QWidget* parent = nullptr);
    ~Menu();

signals:
    void exitClicked();
    void mapSelected(int cityId);
    void mapOpenRequested();

private slots:
    void on_buttonExit_clicked();
    void onOptionClicked(MapOption* opt);
    void onSelectButtonPressed();

private:
    Ui::Menu* ui;
    MapOption* options[4];
    MapOption* current = nullptr;
};

#endif  // MENU_H
