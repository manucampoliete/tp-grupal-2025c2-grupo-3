#include "server.h"

#include <iostream>
#include <utility>

#include "../common/socket/socket.h"

#define END_SERVER_KEY 'q'

Server::Server(const std::string& servname):
    matches_map_monitor(),
    acceptor(servname, matches_map_monitor) {}

int Server::run() {
    acceptor.start();
    while (std::cin.get() != END_SERVER_KEY) {}
    acceptor.stop();
    return EXIT_SUCCESS;
}

Server::~Server() {
    acceptor.join();
}
