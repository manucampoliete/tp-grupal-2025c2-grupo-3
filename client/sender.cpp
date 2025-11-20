#include "sender.h"

#include <iostream>

#include <syslog.h>


Sender::Sender(ClientGameProtocol& protocol, Queue<ActiveDirections>& clientRequestsQueue):
        protocol(protocol), clientRequestsQueue(clientRequestsQueue) {}

void Sender::run() {
    while (shouldKeepRunning()) {
        try {
            /**
             * TODO: (Manu) Moving is not the only action the client can send!
             * We need to handle other actions as well (e.g., modifications, cheats, etc.)
             */ 
            protocol.sendMove(clientRequestsQueue.pop());
        } catch (const std::exception& err) {
            syslog(LOG_INFO, "[Info] Sender: %s", err.what());
            break;
        }
    }

    std::cout << "[SENDER] Thread ended." << std::endl;
}
