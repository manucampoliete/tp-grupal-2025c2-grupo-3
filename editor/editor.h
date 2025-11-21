#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>
#include <QStackedWidget>

#include "menu.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Editor;
}
QT_END_NAMESPACE

class Editor: public QMainWindow {
    Q_OBJECT

public:
    Editor(QWidget* parent = nullptr);
    ~Editor();

private slots:
    void launchMapEditor(int cityId);

private:
    Ui::Editor* ui;
    Menu* menu;

    void exitEditor();
};

#endif  // EDITOR_H
