#include "server.h"

#include <iostream>
#include <utility>

#include "../common/protocol/dummy_protocol.h"
#include "../common/socket/socket.h"

Server::Server(std::string&& servname): servname(std::move(servname)) {}

int Server::run() {
    Socket acceptor(servname.c_str());
    Socket peer = acceptor.accept();
    DummyProtocol protocol(peer);

    std::string send_msg = "Hello, client! You've connected successfully.";
    std::cout << "Sending to client: " << send_msg << "\n";
    protocol.send_message(send_msg);

    std::string recv_msg = protocol.recv_message();
    std::cout << "Received from client: " << recv_msg << "\n";

    return EXIT_SUCCESS;
}

Server::~Server() {}
