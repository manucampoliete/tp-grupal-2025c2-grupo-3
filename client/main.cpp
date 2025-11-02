#include "client.h"
#include <QApplication>
#include "lobby/lobby.h"

#include <iostream>
#include <exception>

#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL.h>

using namespace SDL2pp;

int main(int argc, char* argv[]) {
    try {
        Client client(argv[1], argv[2]);
        client.run(argc, argv);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
