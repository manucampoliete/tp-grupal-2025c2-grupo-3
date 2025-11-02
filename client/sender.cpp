#include "sender.h"

#include <syslog.h>

Sender::Sender(DummyClientProtocol& protocol, Queue<MoveRequest>& client_requests_q):
        protocol(protocol), client_requests_q(client_requests_q) {}

void Sender::run() {
    while (should_keep_running()) {
        try {
            MoveRequest req = client_requests_q.pop();
            protocol.send_move_request(req);
        } catch (const std::exception& err) {
            syslog(LOG_INFO, "[Info] Sender: %s", err.what());
            break;
        }
    }
}
