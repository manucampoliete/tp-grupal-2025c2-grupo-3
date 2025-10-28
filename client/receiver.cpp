#include "receiver.h"
#include "client.h"
#include "../common/commands/move_request.h"

#include <iostream>
#include <syslog.h>

Receiver::Receiver(DummyClientProtocol& protocol, Queue<std::vector<std::pair<ClientID, Vector2D>>>& server_responses_q, Client& client) :
        protocol(protocol),
        server_responses_q(server_responses_q) , client(client) {}

void Receiver::run() {
    while (should_keep_running()) {
        try {
            std::vector<std::pair<ClientID, Vector2D>> resp = protocol.recv_positions_response();
            //server_responses_q.push(resp);

            // le paso los datos al client para que actualice el world
            client.update_world(resp);
        } catch (const std::exception& err) {
            syslog(LOG_INFO, "[Info] Receiver: %s", err.what());
            break;
        }
    }
}
