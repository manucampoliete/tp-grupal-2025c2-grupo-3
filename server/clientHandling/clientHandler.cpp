#include "clientHandler.h"

#include <utility>

#include <sys/socket.h>  // For SHUT_RDWR

#include "../protocol/serverLobbyProtocol.h"
#include "../requestsResolving/lobbyResolver.h"

#include "receiver.h"
#include "sender.h"

void ClientHandler::politeKill() {
    if (receiverPtr) {
        receiverPtr->stop();
    }
    if (senderPtr) {
        senderPtr->stop();
    }
}

void ClientHandler::hardKill() {
    politeKill();
    peer.shutdown(SHUT_RDWR);
    peer.close();
}

void ClientHandler::handleLobbyPhase() {
    ServerLobbyProtocol serverLobbyProtocol(peer, clientId);
    while (shouldKeepRunning() and lobbyResolver.isInLobbyPhase()) {
        serverLobbyProtocol.consumeOne(lobbyResolver);
    }
}

void ClientHandler::launchSenderThread() {
    senderPtr = std::make_unique<Sender>(peer, lobbyResolver.getResponsesQueue());
    senderPtr->start();
}

void ClientHandler::fakeLaunchReceiverThread() {
    receiverPtr =
            std::make_unique<Receiver>(peer, lobbyResolver.getClientCommandsQueue(), clientId);
    receiverPtr->start();
}

ClientHandler::ClientHandler(Socket&& peer, MatchesMapMonitor& matchesMapMonitor,
                             ClientID clientId):
        peer(std::move(peer)),
        lobbyResolver(clientId, matchesMapMonitor),
        clientId(clientId),
        receiverPtr(nullptr),
        senderPtr(nullptr) {}

void ClientHandler::run() {
    handleLobbyPhase();
    if (!shouldKeepRunning()) {
        return;
    }
    launchSenderThread();
    fakeLaunchReceiverThread();
}

void ClientHandler::kill() {
    hardKill();  // Could be changed to politeKill() but it surely requires extra handling in
                 // other parts of the code
}

bool ClientHandler::isDead() const { return !isAlive() and !(senderPtr and senderPtr->isAlive()); }

ClientHandler::~ClientHandler() {}
