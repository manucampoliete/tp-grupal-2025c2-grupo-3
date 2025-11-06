#include "receiver.h"
#include "game_handler.h"

#include <iostream>
#include <syslog.h>

Receiver::Receiver(ClientProtocol& protocol, Queue<Snapshot>& server_snapshots_q, GameHandler& game_handler) :
    protocol(protocol),
    server_snapshots_q(server_snapshots_q),
    game_handler(game_handler) {}


void Receiver::run() {
    while (should_keep_running()) {
        try {
            // leer el tipo de mensaje del servidor
            uint8_t msg_type = protocol.recv_message_type();
            
            switch (msg_type) {
                case MSG_SNAPSHOT: {
                    // recibir snapshot del estado del juego
                    Snapshot snapshot = protocol.recv_snapshot();
                    game_handler.update_world(snapshot);
                    break;
                }
                
                case MSG_COUNTDOWN: {
                    // recibir numero de countdown
                    uint8_t number = protocol.recv_countdown();
                    game_handler.on_countdown(number);
                    break;
                }
                
                case MSG_RACE_START: {
                    // signal de incio de carrera
                    game_handler.on_race_start();
                    break;
                }
                
                case MSG_CHECKPOINT: {
                    // jugador cruzó un checkpoint
                    uint8_t checkpoint_id = protocol.recv_checkpoint();
                    game_handler.on_checkpoint_crossed(checkpoint_id);
                    break;
                }
                
                case MSG_COLLISION: {
                    // notif de colisión
                    CollisionData collision = protocol.recv_collision();
                    game_handler.on_collision(collision);
                    break;
                }
                
                case MSG_PLAYER_DIED: {
                    // un jugador murió
                    uint16_t player_id = protocol.recv_player_died();
                    game_handler.on_player_died(player_id);
                    break;
                }
                
                case MSG_RACE_END: {
                    // terminó la carrera
                    RaceResults results = protocol.recv_race_results();
                    game_handler.on_race_end(results);
                    break;
                }
                
                case MSG_MOD_PHASE: {
                    // fase de modificación
                    CarProperties props = protocol.recv_car_properties();
                    game_handler.on_modification_phase(props);
                    break;
                }
                
                case MSG_GAME_END: {
                    // partida terminada
                    FinalResults results = protocol.recv_final_results();
                    game_handler.on_game_end(results);
                    break;
                }
                
                default:
                    std::cerr << "[RECEIVER] Mensaje desconocido: " << (int)msg_type << std::endl;
                    break;
            }
            
        } catch (const std::exception& err) {
            syslog(LOG_INFO, "[Info] Receiver: %s", err.what());
            break;
        }
    }
    
    std::cout << "[RECEIVER] Hilo detenido." << std::endl;
}

