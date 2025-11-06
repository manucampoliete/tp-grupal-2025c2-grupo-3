#include "sender.h"

#include <syslog.h>
#include <iostream>


Sender::Sender(ClientProtocol& protocol, Queue<MoveRequest>& client_requests_q) :
        protocol(protocol), client_requests_q(client_requests_q) {}

void Sender::run() {
    while (shouldKeepRunning()) {
        try {
            MoveRequest req = client_requests_q.pop();
            protocol.send_move(req);
        } catch (const std::exception& err) {
            syslog(LOG_INFO, "[Info] Sender: %s", err.what());
            break;
        }
    }

    std::cout << "[SENDER] Hilo detenido." << std::endl;
}
