#include "sender.h"

#include <iostream>

#include <syslog.h>


Sender::Sender(ClientGameProtocol& protocol, Queue<ClientMessage>& commandQueue):
        protocol(protocol), commandQueue(commandQueue) {}

void Sender::run() {
    while (shouldKeepRunning()) {
        try {
            ClientMessage cmd = commandQueue.pop();

            std::visit([this](auto&& command) {
                using T = std::decay_t<decltype(command)>;
                
                if constexpr (std::is_same_v<T, MoveCommand>)
                    protocol.sendMove(command.directions);
                else if constexpr (std::is_same_v<T, CheatInmortalityCommand>) 
                    protocol.sendInmortalityRequest();
                else if constexpr (std::is_same_v<T, CheatInstaWinCommand>) 
                    protocol.sendInstaWinRequest();
                else if constexpr (std::is_same_v<T, CheatInstaLoseCommand>) 
                    protocol.sendInstaLoseRequest();
                else if constexpr (std::is_same_v<T, ModifyCarCommand>) 
                    protocol.sendModifications(command.speedMod, command.healthMod);
            }, cmd);

        } catch (const ClosedQueue& e) {
            std::cout << "[SENDER] Queue closed, ending..." << std::endl;
            break;
        } catch (const std::exception& err) {
            std::cerr << "[SENDER] Error: " << err.what() << std::endl;
            syslog(LOG_INFO, "[Info] Sender: %s", err.what());
            break;
        }
    }

    std::cout << "[SENDER] Thread ended." << std::endl;
}
