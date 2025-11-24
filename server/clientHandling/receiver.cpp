#include "receiver.h"

#include <syslog.h>

Receiver::Receiver(Socket& skt, Queue<std::unique_ptr<Command>>& clientCommandsQueue,
                   ClientID clientId):
    gameResolver(clientId, clientCommandsQueue), protocol(skt, gameResolver), _keepRunning(true) {
    run();
}

void Receiver::stop() { _keepRunning = false; }

void Receiver::run() {
    while (_keepRunning) {
        try {
            protocol.consumeOne();
        } catch (const std::exception& err) {
            syslog(LOG_INFO, "[Info] Receiver: %s", err.what());
            break;
        }
    }
}
