#include "receiver.h"
#include "../common/commands/move_request.h"

#include <syslog.h>

Receiver::Receiver(DummyServerProtocol& protocol, Queue<MoveRequestWithID>& client_commands_q, ClientID client_id):
        protocol(protocol),
        client_commands_q(client_commands_q),
        client_id(client_id) {}

void Receiver::run() {
    while (should_keep_running()) {
        try {
            MoveRequest req = protocol.recv_move_request();
            client_commands_q.push(MoveRequestWithID(req, client_id));
        } catch (const std::exception& err) {
            syslog(LOG_INFO, "[Info] Receiver: %s", err.what());
            break;
        }
    }
}
