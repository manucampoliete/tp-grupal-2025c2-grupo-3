#include "clientHandler.h"

#include <utility>

#include <sys/socket.h>  // For SHUT_RDWR

#include "../protocol/serverLobbyProtocol.h"
#include "../requestsResolving/lobbyResolver.h"

void ClientHandler::politeKill() {
    Thread::stop();  // If in lobby phase, stop this thread

    // If in game phase, stop sender thread and this thread (which is running in receiver object)
    if (sender)
        sender->stop();
    if (receiver)
        receiver->stop();
}

void ClientHandler::hardKill() {
    politeKill();
    peer.shutdown(SHUT_RDWR);
    peer.close();
}

void ClientHandler::handleLobbyPhase() {
    ServerLobbyProtocol serverLobbyProtocol(peer, lobbyResolver, clientId);
    while (shouldKeepRunning() and lobbyResolver.isInLobbyPhase()) {
        serverLobbyProtocol.consumeOne();
    }
}

ClientHandler::ClientHandler(Socket&& peer, MatchesMapMonitor& matchesMapMonitor,
                             ClientID clientId):
        peer(std::move(peer)),
        lobbyResolver(clientId, matchesMapMonitor),
        clientId(clientId),
        sender(std::nullopt),
        receiver(std::nullopt) {}

void ClientHandler::join() {
    Thread::join();
    if (sender)
        sender->join();
}

void ClientHandler::kill() {
    if (!isDead()) hardKill();  // If dead, not necessary to kill

    /**
     * NOTE: could be changed to politeKill() but it surely 
     * requires extra handling in other parts of the code
     */
}

bool ClientHandler::isDead() const { return !isAlive() and (!sender or !sender->isAlive()); }

void ClientHandler::run() {
    handleLobbyPhase();
    if (!shouldKeepRunning()) {
        return;
    }
    sender.emplace(peer, lobbyResolver.getResponsesQueue());
    receiver.emplace(peer, lobbyResolver.getClientCommandsQueue(), clientId);
}

ClientHandler::~ClientHandler() {
    kill();
    join();
}
