#include "receiver.h"
#include "../../common/errors/peerDisconnectedError.h"

#include <iostream>
#include <syslog.h>

Receiver::Receiver(Socket& skt, Queue<ServerMessage>& serverMessagesQueue):
    protocol(skt),
    serverMessagesQueue(serverMessagesQueue) {}

void Receiver::run() {
    std::cout << "[RECEIVER] Thread started, waiting for messages..." << std::endl;

    while (shouldKeepRunning()) {
        try {
            uint8_t msgType = protocol.recvMessageType();

            std::cout << "[RECEIVER] Message received: 0x" << std::hex << (int)msgType << std::dec
                      << " (" << (int)msgType << ")" << std::endl;

            // Cada tipo de mensaje se empaqueta en su struct y se pushea a la queue
            switch (msgType) {
                case SEND_RACE_SNAPSHOT: {
                    std::cout << "[RECEIVER] → Processing SNAPSHOT" << std::endl;
                    serverMessagesQueue.push(protocol.recvSnapshot());
                    break;
                }

                case MSG_COUNTDOWN: {
                    std::cout << "[RECEIVER] → Processing COUNTDOWN" << std::endl;
                    CountdownMessage msg;
                    msg.number = protocol.recvCountdown();
                    serverMessagesQueue.push(msg);
                    break;
                }

                case MSG_RACE_INFO: {
                    std::cout << "[RECEIVER] → Processing RACE_INFO" << std::endl;
                    RaceInfoMessage msg;
                    msg.info = protocol.recvRaceInfo();
                    serverMessagesQueue.push(msg);
                    break;
                }

                case MSG_RACE_START: {
                    std::cout << "[RECEIVER] → Processing RACE_START" << std::endl;
                    serverMessagesQueue.push(RaceStartMessage{});
                    break;
                }

                case MSG_RACE_END: {
                    std::cout << "[RECEIVER] → Processing RACE_END" << std::endl;
                    RaceEndMessage msg;
                    msg.results = protocol.recvRaceResults();
                    serverMessagesQueue.push(msg);
                    break;
                }

                case MSG_STATS_COUNTDOWN: {
                    std::cout << "[RECEIVER] → Processing STATS_COUNTDOWN" << std::endl;
                    StatsCountdownMessage msg;
                    msg.number = protocol.recvStatsCountdown();
                    serverMessagesQueue.push(msg);
                    break;
                }

                case MSG_MOD_PHASE: {
                    std::cout << "[RECEIVER] → Processing MOD_PHASE" << std::endl;
                    ModificationPhaseMessage msg;
                    msg.properties = protocol.recvCarProperties();
                    serverMessagesQueue.push(msg);
                    break;
                }

                case MSG_MOD_COUNTDOWN: {
                    std::cout << "[RECEIVER] → Processing MOD_COUNTDOWN" << std::endl;
                    ModCountdownMessage msg;
                    msg.number = protocol.recvModCountdown();
                    serverMessagesQueue.push(msg);
                    break;
                }

                case MSG_GAME_END: {
                    std::cout << "[RECEIVER] → Processing GAME_END" << std::endl;
                    GameEndMessage msg;
                    msg.results = protocol.recvFinalResults();
                    serverMessagesQueue.push(msg);
                    break;
                }

                case MSG_COLLISION: {
                    std::cout << "[RECEIVER] → Processing COLLISION" << std::endl;
                    CollisionMessage msg;
                    msg.data = protocol.recvCollision();
                    serverMessagesQueue.push(msg);
                    break;
                }

                case MSG_PLAYER_DIED: {
                    std::cout << "[RECEIVER] → Processing PLAYER_DIED" << std::endl;
                    PlayerDiedMessage msg;
                    msg.playerId = protocol.recvPlayerDied();
                    serverMessagesQueue.push(msg);
                    break;
                }

                default:
                    std::cerr << "[RECEIVER] UNKNOWN MESSAGE: " << (int)msgType << std::endl;
                    break;
            }
        
        } catch (const PeerDisconnectedError& e) {
            std::cout << "[RECEIVER] Peer disconnected, ending..." << std::endl;
            serverMessagesQueue.close();
            break;
        } catch (const ClosedQueue& e) {
            std::cout << "[RECEIVER] Queue closed, ending..." << std::endl;
            break;
        } catch (const std::exception& err) {
            std::cerr << "[RECEIVER] Error: " << err.what() << std::endl;
            syslog(LOG_INFO, "[Info] Receiver: %s", err.what());
            break;
        }
    }

    std::cout << "[RECEIVER] Thread ended." << std::endl;
}
