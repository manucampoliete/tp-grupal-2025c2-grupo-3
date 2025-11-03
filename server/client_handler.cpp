#include "client_handler.h"

#include <iostream>
#include <sstream>
#include <utility>

#include <sys/socket.h>  // For SHUT_RDWR

void ClientHandler::polite_kill() {
    receiver.stop();  // should_keep_running() = false
    // sender.stop();    // should_keep_running() = false
}

void ClientHandler::hard_kill() {
    polite_kill();
    peer.shutdown(SHUT_RDWR);
    peer.close();
}

ClientHandler::ClientHandler(Socket&& peer, MatchesMapMonitor& matches_map_monitor, ClientID client_id):
        peer(std::move(peer)),
        protocol(this->peer),
        sender(protocol),
        receiver(protocol, matches_map_monitor, client_id, sender),
        client_id(client_id) // Necessary?
        {}

void ClientHandler::start() {
    receiver.start();
    // Sender is started by the Receiver when the response queue is set
}

void ClientHandler::join() {
    receiver.join();
    sender.join();
}

void ClientHandler::kill() {
    hard_kill();  // Could be changed to polite_kill() but it surely requires extra handling in
                  // other parts of the code
}

bool ClientHandler::is_dead() const { return !receiver.is_alive() and !sender.is_alive(); }

ClientHandler::~ClientHandler() {}
