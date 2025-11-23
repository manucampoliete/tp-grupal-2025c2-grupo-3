#include "sender.h"

#include <syslog.h>

Sender::Sender(Socket& skt, Queue<std::shared_ptr<Snapshot>>& responsesQueue):
    protocol(skt), responsesQueue(responsesQueue) {
    start();
}

void Sender::run() {
    while (shouldKeepRunning()) {
        try {
            protocol.sendSnapshot(responsesQueue.pop());
        } catch (const std::exception& err) {
            syslog(LOG_INFO, "[Info] Sender: %s", err.what());
            break;
        }
    }
}
