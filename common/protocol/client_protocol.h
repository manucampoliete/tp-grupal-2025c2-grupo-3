#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include "../socket/socket.h"
#include "../commands/move_request.h"
#include "game_data.h"

#include <cstdint>
#include <string>

class ClientProtocol {
private:
    Socket& socket;

    // Send methods
    void send_u8(uint8_t value);
    void send_u16(uint16_t value);
    void send_u32(uint32_t value);
    void send_string(const std::string& str);

    // Receive methods
    uint8_t recv_u8();
    uint16_t recv_u16();
    uint32_t recv_u32();
    std::string recv_string();
    
    // Handshake methods
    void recv_initial_info();

public:
    explicit ClientProtocol(Socket& socket);

    uint16_t send_create(const std::string& username, uint8_t car_id);

    bool send_join(uint16_t match_id, const std::string& username, uint8_t car_id);

    bool send_start();

    void recv_start_signal();


    // GAME
    
    // lee el tipo de mensaje del servidor y devuelve el código
    uint8_t recv_message_type();
    
    // envía un input de movimiento al servidor
    void send_move(const MoveRequest& request);
    
    // envía modificaciones del auto al servidor
    void send_modifications(bool speed_mod, bool accel_mod);
    
    // recibe un snapshot del estado del juego
    Snapshot recv_snapshot();
    
    // recibe número de countdown (3, 2, 1, 0=GO)
    uint8_t recv_countdown();
    
    // recibe ID de checkpoint cruzado
    uint8_t recv_checkpoint();
    
    // recibe datos de colisión
    CollisionData recv_collision();
    
    // recibe ID del jugador que murió
    uint16_t recv_player_died();
    
    // recibe resultados de la carrera
    RaceResults recv_race_results();
    
    // recibe propiedades del auto para modificación
    CarProperties recv_car_properties();
    
    // recibe resultados finales de la partida
    FinalResults recv_final_results();

};
#endif // CLIENT_PROTOCOL_H
