#include "sender.h"
#include "../../common/errors/peerDisconnectedError.h"

#include <iostream>
#include <syslog.h>

Sender::Sender(Socket& skt, Queue<ClientMessage>& commandQueue):
        protocol(skt), commandQueue(commandQueue) {}

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
                else if constexpr (std::is_same_v<T, CheatSuperSpeedCommand>) 
                    protocol.sendSuperSpeedRequest();
                else if constexpr (std::is_same_v<T, ModifyCarCommand>) 
                    protocol.sendModifications(command.speedMod, command.healthMod, command.accelMod, command.massMod);
            }, cmd);
        
        } catch (const PeerDisconnectedError& e) {
            commandQueue.close();
            break;
        } catch (const ClosedQueue& e) {
            break;
        } catch (const std::exception& err) {
            syslog(LOG_INFO, "[Info] Sender: %s", err.what());
            break;
        }
    }
}
