#ifndef CLIENT_H
#define CLIENT_H

#include <QApplication>
#include "lobby/lobby.h"

class Client {
private:
    //ClientProtocol clprotocol;
    //int id;

public:
    explicit Client(const char* hostname, const char* servname);

    int run(int argc, char* argv[]);
};

#endif // CLIENT_H