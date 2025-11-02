#include "receiver.h"
#include "../common/commands/move_request.h"

#include <syslog.h>

Receiver::Receiver(DummyClientProtocol& protocol, Queue<std::vector<std::pair<ClientID, Vector2D>>>& server_responses_q):
        protocol(protocol),
        server_responses_q(server_responses_q) {}

void Receiver::run() {
    while (should_keep_running()) {
        try {
            std::vector<std::pair<ClientID, Vector2D>> resp = protocol.recv_positions_response();
            server_responses_q.push(resp);
        } catch (const std::exception& err) {
            syslog(LOG_INFO, "[Info] Receiver: %s", err.what());
            break;
        }
    }
}
