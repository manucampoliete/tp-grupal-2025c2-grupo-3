#include "server.h"

#include "../common/protocol/dummy_protocol.h"
#include "../common/socket/socket.h"

#include <iostream>
#include <utility>

#define END_SERVER_KEY 'q'

Server::Server(const std::string& servname):
    acceptor(std::move(Socket(servname.c_str()))) {}

int Server::run() {
    acceptor.start();
    while (std::cin.get() != END_SERVER_KEY) {}
    acceptor.stop();
    acceptor.join();
    return EXIT_SUCCESS;
}

Server::~Server() {}
