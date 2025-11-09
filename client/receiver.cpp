#include "receiver.h"
#include "game_handler.h"

#include <iostream>
#include <syslog.h>

Receiver::Receiver(ClientProtocol& protocol, Queue<Snapshot>& server_snapshots_q, GameHandler& game_handler) :
    protocol(protocol),
    server_snapshots_q(server_snapshots_q),
    game_handler(game_handler) {}


void Receiver::run() {
    std::cout << "[RECEIVER] Hilo iniciado, esperando mensajes..." << std::endl;
    
    while (shouldKeepRunning()) {
        try {
            // leer el tipo de mensaje del servidor
            uint8_t msg_type = protocol.recv_message_type();
            
            std::cout << "[RECEIVER] Mensaje recibido: 0x" << std::hex << (int)msg_type << std::dec << " (" << (int)msg_type << ")" << std::endl;
            
            switch (msg_type) {
                case SEND_SNAPSHOT: {
                    std::cout << "[RECEIVER] → Procesando SNAPSHOT" << std::endl;
                    Snapshot snapshot = protocol.recv_snapshot();
                    game_handler.update_world(snapshot);
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