#include "sender.h"

#include <syslog.h>

Sender::Sender(ServerProtocol& protocol):
        protocol(protocol),
        responses_q(nullptr) {}

void Sender::set_responses_queue(Queue<Snapshot>* responses_q) {
    this->responses_q = responses_q;
}

void Sender::run() {
    while (should_keep_running()) {
        try {
            Snapshot snapshot = responses_q->pop();
            protocol.send_snapshot(snapshot);
        } catch (const std::exception& err) {
            syslog(LOG_INFO, "[Info] Sender: %s", err.what());
            break;
        }
    }
}
