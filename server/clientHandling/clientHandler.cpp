#include "clientHandler.h"
#include "receiver.h"
#include "sender.h"
#include "../protocol/serverLobbyProtocol.h"
#include "../requestsResolving/lobbyResolver.h"

#include <utility>
#include <sys/socket.h>  // For SHUT_RDWR

void ClientHandler::politeKill() {
    if (receiver_ptr) {
        receiver_ptr->stop();
    }
    if (sender_ptr) {
        sender_ptr->stop();
    }
}

void ClientHandler::hardKill() {
    politeKill();
    peer.shutdown(SHUT_RDWR);
    peer.close();
}

ClientHandler::ClientHandler(Socket&& peer, MatchesMapMonitor& matches_map_monitor,
                             ClientID client_id):
        peer(std::move(peer)),
        lobbyResolver(client_id, matches_map_monitor),
        client_id(client_id),
        receiver_ptr(nullptr),
        sender_ptr(nullptr) {}

void ClientHandler::run() {
    ServerLobbyProtocol serverLobbyProtocol(peer);
    while (shouldKeepRunning() && lobbyResolver.isInLobbyPhase()) {
        serverLobbyProtocol.consumeOne(lobbyResolver);
    }

    sender_ptr = std::make_unique<Sender>(peer, lobbyResolver.getResponsesQueue());
    sender_ptr->start();

    receiver_ptr = std::make_unique<Receiver>(peer, lobbyResolver.getClientCommandsQueue(), client_id);
    receiver_ptr->start();
}

void ClientHandler::kill() {
    hardKill();  // Could be changed to politeKill() but it surely requires extra handling in
                 // other parts of the code
}

bool ClientHandler::isDead() const { 
    return !(receiver_ptr && receiver_ptr->isAlive()) &&
           !(sender_ptr && sender_ptr->isAlive());
}

ClientHandler::~ClientHandler() {}
