#include "server.h"

#include <iostream>

#define END_SERVER_KEY 'q'

Server::Server(const std::string& servname):
    matchesMapMonitor(), acceptor(servname, matchesMapMonitor) {}

int Server::run() {
    acceptor.start();
    while (std::cin.get() != END_SERVER_KEY) {}
    acceptor.stop();
    // matchesMapMonitor.stopAllMatches();
    return EXIT_SUCCESS;
}

Server::~Server() { acceptor.join(); }
