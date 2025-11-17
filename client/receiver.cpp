#include "receiver.h"

#include <iostream>

#include <syslog.h>


Receiver::Receiver(ClientProtocol& protocol, Queue<Snapshot>& serverSnapshotsQueue, GameLoop& gameLoop):
    protocol(protocol),
    serverSnapshotsQueue(serverSnapshotsQueue),
    gameLoop(gameLoop),
    lastCountdownNumber(UINT8_MAX)  // Dummy value to force the first update
    {}


void Receiver::run() {
    std::cout << "[RECEIVER] Thread started, waiting for messages..." << std::endl;

    while (shouldKeepRunning()) {
        try {
            uint8_t msgType = protocol.recvMessageType();

            std::cout << "[RECEIVER] Message received: 0x" << std::hex << (int)msgType << std::dec
                      << " (" << (int)msgType << ")" << std::endl;

            // We don't use the queue for specific events
            // We just use it for race snapshots

            /**
             * TODO: (Manu) We'll probably have to use the queue all the time, even for 'specific' events
             */

            switch (msgType) {
                case SEND_SNAPSHOT: {
                    std::cout << "[RECEIVER] → Processing SNAPSHOT" << std::endl;
                    serverSnapshotsQueue.push(protocol.recvSnapshot());
                    break;
                }

                case MSG_COUNTDOWN: {
                    std::cout << "[RECEIVER] → Processing COUNTDOWN" << std::endl;
                    uint8_t number = protocol.recvCountdown();
                    if (number != lastCountdownNumber) {
                        std::cout << "[RECEIVER] → Countdown changed: " << (int)number << std::endl;
                        gameLoop.onCountdown(number);
                        lastCountdownNumber = number;
                    }
                    // If it's the same number, ignore it
                    break;
                }

                case MSG_RACE_START: {
                    std::cout << "[RECEIVER] → Processing RACE_START" << std::endl;
                    gameLoop.onRaceStart();
                    break;
                }

                case MSG_RACE_END: {
                    std::cout << "[RECEIVER] → Processing RACE_END" << std::endl;
                    gameLoop.onRaceEnd(protocol.recvRaceResults());
                    break;
                }

                case MSG_MOD_PHASE: {
                    std::cout << "[RECEIVER] → Processing MOD_PHASE" << std::endl;
                    gameLoop.onModificationPhase(protocol.recvCarProperties());
                    break;
                }

                case MSG_GAME_END: {
                    std::cout << "[RECEIVER] → Processing GAME_END" << std::endl;
                    gameLoop.onGameEnd(protocol.recvFinalResults());
                    break;
                }

                case MSG_COLLISION: {
                    std::cout << "[RECEIVER] → Processing COLLISION" << std::endl;
                    gameLoop.onCollision(protocol.recvCollision());
                    break;
                }

                case MSG_PLAYER_DIED: {
                    std::cout << "[RECEIVER] → Processing PLAYER_DIED" << std::endl;
                    gameLoop.onPlayerDied(protocol.recvPlayerDied());
                    break;
                }

                case MSG_CHECKPOINT: {
                    std::cout << "[RECEIVER] → Processing CHECKPOINT" << std::endl;
                    gameLoop.onCheckpointCrossed(protocol.recvCheckpoint());
                    break;
                }

                default:
                    std::cerr << "[RECEIVER] UNKNOWN MESSAGE: " << (int)msgType << std::endl;
                    break;
            }

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
