#include "receiver.h"

#include <iostream>

#include <syslog.h>

#include "game_handler.h"

Receiver::Receiver(ClientProtocol& protocol, Queue<Snapshot>& server_snapshots_q,
                   GameHandler& game_handler):
        protocol(protocol), server_snapshots_q(server_snapshots_q), game_handler(game_handler) {}


void Receiver::run() {
    std::cout << "[RECEIVER] Hilo iniciado, esperando mensajes..." << std::endl;

    while (shouldKeepRunning()) {
        try {
            // leer el tipo de mensaje del servidor
            uint8_t msg_type = protocol.recv_message_type();

            std::cout << "[RECEIVER] Mensaje recibido: 0x" << std::hex << (int)msg_type << std::dec
                      << " (" << (int)msg_type << ")" << std::endl;

            switch (msg_type) {
                case SEND_SNAPSHOT: {
                    std::cout << "[RECEIVER] → Procesando SNAPSHOT" << std::endl;
                    Snapshot snapshot = protocol.recv_snapshot();
                    game_handler.update_world(snapshot);
                    break;
                }

                case MSG_COUNTDOWN: {
                    std::cout << "[RECEIVER] → Procesando COUNTDOWN" << std::endl;
                    uint8_t number = protocol.recv_countdown();
                    game_handler.on_countdown(number);
                    break;
                }

                case MSG_RACE_START: {
                    std::cout << "[RECEIVER] → Procesando RACE_START" << std::endl;
                    game_handler.on_race_start();
                    break;
                }

                case MSG_RACE_END: {
                    std::cout << "[RECEIVER] → Procesando RACE_END" << std::endl;
                    RaceResults results = protocol.recv_race_results();
                    game_handler.on_race_end(results);
                    break;
                }

                case MSG_MOD_PHASE: {
                    std::cout << "[RECEIVER] → Procesando MOD_PHASE" << std::endl;
                    CarProperties props = protocol.recv_car_properties();
                    game_handler.on_modification_phase(props);
                    break;
                }

                case MSG_GAME_END: {
                    std::cout << "[RECEIVER] → Procesando GAME_END" << std::endl;
                    FinalResults results = protocol.recv_final_results();
                    game_handler.on_game_end(results);
                    break;
                }

                case MSG_COLLISION: {
                    std::cout << "[RECEIVER] → Procesando COLLISION" << std::endl;
                    CollisionData collision = protocol.recv_collision();
                    game_handler.on_collision(collision);
                    break;
                }

                case MSG_PLAYER_DIED: {
                    std::cout << "[RECEIVER] → Procesando PLAYER_DIED" << std::endl;
                    uint16_t dead_player_id = protocol.recv_player_died();
                    game_handler.on_player_died(dead_player_id);
                    break;
                }

                case MSG_CHECKPOINT: {
                    std::cout << "[RECEIVER] → Procesando CHECKPOINT" << std::endl;
                    uint8_t checkpoint_id = protocol.recv_checkpoint();
                    game_handler.on_checkpoint_crossed(checkpoint_id);
                    break;
                }
                
                default:
                    std::cerr << "[RECEIVER] MENSAJE DESCONOCIDO: " << (int)msg_type << std::endl;
                    break;
            }

        } catch (const ClosedQueue& e) {
            std::cout << "[RECEIVER] Cola cerrada, finalizando..." << std::endl;
            break;
        } catch (const std::exception& err) {
            std::cerr << "[RECEIVER] Error: " << err.what() << std::endl;
            syslog(LOG_INFO, "[Info] Receiver: %s", err.what());
            break;
        }
    }

    std::cout << "[RECEIVER] Hilo detenido." << std::endl;
}
