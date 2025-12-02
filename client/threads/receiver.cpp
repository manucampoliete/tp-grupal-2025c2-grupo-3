#include "receiver.h"
#include "../../common/errors/peerDisconnectedError.h"

#include <iostream>
#include <syslog.h>

Receiver::Receiver(Socket& skt, Queue<ServerMessage>& serverMessagesQueue):
    protocol(skt),
    serverMessagesQueue(serverMessagesQueue) {}

void Receiver::run() {
    while (shouldKeepRunning()) {
        try {
            uint8_t msgType = protocol.recvMessageType();

            // Cada tipo de mensaje se empaqueta en su struct y se pushea a la queue
            switch (msgType) {
                case SEND_RACE_SNAPSHOT: {
                    serverMessagesQueue.push(protocol.recvSnapshot());
                    break;
                }

                case MSG_COUNTDOWN: {
                    CountdownMessage msg;
                    msg.number = protocol.recvCountdown();
                    serverMessagesQueue.push(msg);
                    break;
                }

                case MSG_RACE_INFO: {
                    RaceInfoMessage msg;
                    msg.info = protocol.recvRaceInfo();
                    serverMessagesQueue.push(msg);
                    break;
                }

                case MSG_RACE_START: {
                    serverMessagesQueue.push(RaceStartMessage{});
                    break;
                }

                case MSG_RACE_END: {
                    RaceEndMessage msg;
                    msg.results = protocol.recvRaceResults();
                    serverMessagesQueue.push(msg);
                    break;
                }

                case MSG_STATS_COUNTDOWN: {
                    StatsCountdownMessage msg;
                    msg.number = protocol.recvStatsCountdown();
                    serverMessagesQueue.push(msg);
                    break;
                }

                case MSG_MOD_PHASE: {
                    ModificationPhaseMessage msg;
                    msg.properties = protocol.recvCarProperties();
                    serverMessagesQueue.push(msg);
                    break;
                }

                case MSG_MOD_COUNTDOWN: {
                    ModCountdownMessage msg;
                    msg.number = protocol.recvModCountdown();
                    serverMessagesQueue.push(msg);
                    break;
                }

                case MSG_GAME_END: {
                    GameEndMessage msg;
                    msg.results = protocol.recvFinalResults();
                    serverMessagesQueue.push(msg);
                    break;
                }

                case MSG_COLLISION: {
                    CollisionMessage msg;
                    msg.data = protocol.recvCollision();
                    serverMessagesQueue.push(msg);
                    break;
                }

                case MSG_PLAYER_DIED: {
                    PlayerDiedMessage msg;
                    msg.playerId = protocol.recvPlayerDied();
                    serverMessagesQueue.push(msg);
                    break;
                }

                default:
                    break;
            }
        
        } catch (const PeerDisconnectedError& e) {
            serverMessagesQueue.close();
            break;
        } catch (const ClosedQueue& e) {
            break;
        } catch (const std::exception& err) {
            syslog(LOG_INFO, "[Info] Receiver: %s", err.what());
            break;
        }
    }
}
