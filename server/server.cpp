#include "server.h"

#include <iostream>

#define END_SERVER_KEY 'q'

Server::Server(const std::string& servname):
        matchesMapMonitor(), acceptor(servname, matchesMapMonitor) {}

int Server::run() {
    while (std::cin.get() != END_SERVER_KEY) {}
    matchesMapMonitor.stopAllMatches();
    return EXIT_SUCCESS;
}

Server::~Server() {
    matchesMapMonitor.joinAllMatches();
}
