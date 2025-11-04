#include "server.h"

#include <iostream>
#include <utility>

#include "../common/socket/socket.h"

#define END_SERVER_KEY 'q'

Server::Server(const std::string& servname):
        game(),
        acceptor(std::move(Socket(servname.c_str())), game.get_client_commands_queue(),
                 game.get_response_queues_monitor()) {}

int Server::run() {
    game.start();
    acceptor.start();
    while (std::cin.get() != END_SERVER_KEY) {}
    game.stop();
    acceptor.stop();
    return EXIT_SUCCESS;
}

Server::~Server() {
    game.join();
    acceptor.join();
}
