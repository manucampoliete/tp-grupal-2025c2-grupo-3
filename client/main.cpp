#include "common/foo.h"
#include <QApplication>
#include "lobby/mainwindow.h"

#include <iostream>
#include <exception>

#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL.h>

using namespace SDL2pp;

int main(int argc, char *argv[]) try {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
} catch (std::exception& e) {
	// If case of error, print it and exit with error
	std::cerr << e.what() << std::endl;
	return 1;
}
