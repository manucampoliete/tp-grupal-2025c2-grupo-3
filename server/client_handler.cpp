#include "client_handler.h"

#include <iostream>
#include <sstream>
#include <utility>

#include <sys/socket.h>  // For SHUT_RDWR

void ClientHandler::polite_kill() {
    receiver.stop();  // should_keep_running() = false
    sender.stop();    // should_keep_running() = false
}

void ClientHandler::hard_kill() {
    polite_kill();
    peer.shutdown(SHUT_RDWR);
    peer.close();
}

ClientHandler::ClientHandler(Socket&& peer, Queue<MoveRequestWithID>& client_commands_q,
                             ResponseQueuesMonitor& response_queues, ClientID client_id):
        peer(std::move(peer)),
        protocol(this->peer),
        receiver(protocol, client_commands_q,
                 client_id),  // Inject a ref to the client commands queue to the Receiver!
        sender(protocol, response_queues.add_queue(
                                 client_id)),  // Inject a ref to a response queue to the Sender!
        response_queues(response_queues),
        client_id(client_id) {}

void ClientHandler::start() {
    receiver.start();
    sender.start();
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

ClientHandler::~ClientHandler() { response_queues.remove_queue(client_id); }
