#include "sender.h"

#include <syslog.h>

Sender::Sender(DummyServerProtocol& protocol, Queue<std::vector<std::pair<ClientID, Vector2D>>>& responses_q):
        protocol(protocol), responses_q(responses_q) {}

void Sender::run() {
    while (should_keep_running()) {
        try {
            std::vector<std::pair<ClientID, Vector2D>> resp = responses_q.pop();
            protocol.send_positions_response(resp);
        } catch (const std::exception& err) {
            syslog(LOG_INFO, "[Info] Sender: %s", err.what());
            break;
        }
    }
}
