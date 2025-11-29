#include <QApplication>
#include <QFontDatabase>
#include "editor.h"

int main(int argc, char *argv[]){
    QApplication app(argc, argv);

    Editor window;
    window.show();

    return app.exec();
}
