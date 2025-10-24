#include "server.h"

#include "../common/protocol/dummy_protocol.h"
#include "../common/socket/socket.h"

#include <iostream>
#include <utility>

Server::Server(std::string&& servname): servname(std::move(servname)) {}

int Server::run() {
    
    Socket acceptor(servname.c_str());

    while (true) {
        try {
            Socket peer = acceptor.accept();
            std::cout << "[Server] Client connected!" << std::endl;

            DummyProtocol protocol(peer);

            while (true) {
                std::string message = protocol.recv_message();
                std::cout << "[Server] Received message: " << message << std::endl;

                if (message.empty()) {
                    // Connection closed by client
                    break;
                }

                std::string response = "Echo: " + message;
                protocol.send_message(response);
                std::cout << "[Server] Sent response: " << response << std::endl;
            }

            std::cout << "[Server] Client disconnected." << std::endl;

        } catch (const std::exception& e) {
            std::cerr << "[Server] Error: " << e.what() << std::endl;
        }
    }

    return EXIT_SUCCESS;
}

Server::~Server() {}
