#include "receiver.h"

#include <syslog.h>

Receiver::Receiver(Socket& skt, ClientID clientId,
                   Queue<std::unique_ptr<Command>>& clientCommandsQueue):
        protocol(skt), gameResolver(clientId, clientCommandsQueue) {}

void Receiver::start() { run(); }

void Receiver::run() {
    while (shouldKeepRunning()) {
        try {
            protocol.consumeOne(gameResolver);
        } catch (const std::exception& e) {
            syslog(LOG_INFO, "[Info] Receiver: %s", e.what());
            break;
        }
    }
}
