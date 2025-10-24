#include "client_handler.h"

#include <utility>
#include <sys/socket.h>  // For SHUT_RDWR
#include <iostream>
#include <sstream>

void ClientHandler::polite_kill() {
    Thread::stop();  // should_keep_running() = false
}

void ClientHandler::hard_kill() {
    polite_kill();
    peer.shutdown(SHUT_RDWR);
    peer.close();
}

ClientHandler::ClientHandler(Socket&& peer, ClientID client_id):
        peer(std::move(peer)),
        protocol(this->peer),
        client_id(client_id) {}

void ClientHandler::run() {
    while (should_keep_running()) {
        try {
            std::string message = protocol.recv_message();
            std::ostringstream oss;
            oss << "[Server] Message received from client " << client_id << ": " << message << std::endl;
            std::cout << oss.str();  // Atomic

            if (message.empty()) {
                // Connection closed by client
                break;
            }

            protocol.send_message(message);  // Echo
            oss = std::ostringstream();
            oss << "[Server] Response sent to client " << client_id << ": " << message << std::endl;
            std::cout << oss.str();  // Atomic

        } catch (const std::exception& e) {
            std::ostringstream oss;
            oss << "[Server] Error with client " << client_id << ": " << e.what() << std::endl;
            std::cerr << oss.str();  // Atomic
            break;
        }
    }

    std::ostringstream oss;
    oss << "[Server] Client " << client_id << " has disconnected." << std::endl;
    std::cout << oss.str();  // Atomic
}

void ClientHandler::kill() {
    hard_kill();  // Could be changed to polite_kill() but it surely requires extra handling in
                  // other parts of the code
}

bool ClientHandler::is_dead() const { return !Thread::is_alive(); }

ClientHandler::~ClientHandler() {}
