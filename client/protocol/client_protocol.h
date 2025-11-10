#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include "../../common/socket/socket.h"
// #include "../../common/utils/activeDirections.h"
#include "../../common/utils/activeDirections.h"
#include "../../common/utils/carinfo.h"
#include "../../common/messages/snapshot.h"
#include "../../common/messages/game_data.h"
#include "../../common/types/types.h"

#include "../../common/protocol/sendProtocol.h"
#include "../../common/protocol/recvProtocol.h"

#include <cstdint>
#include <string>

class ClientProtocol: public SendProtocol, public RecvProtocol {
private:


public:
    explicit ClientProtocol(Socket& socket);

    // Handshake methods
    std::vector<CarInfo> recv_initial_info();

    uint16_t send_create(const std::string& username, uint8_t car_id);

    bool send_join(uint16_t match_id, const std::string& username, uint8_t car_id);

    void send_start();

    void recv_start_signal();


    // GAME

    ClientID recv_client_id();
    
    // lee el tipo de mensaje del servidor y devuelve el código
    uint8_t recv_message_type();
    
    // envía un input de movimiento al servidor
    // void send_move(const ActiveDirections& request);
    void send_move(const ActiveDirections& request);
    
    // recibe un snapshot del estado del juego (ahora usando snapshot.h)
    Snapshot recv_snapshot();

    std::string recv_username();
    
    // envía modificaciones del auto al servidor
    void send_modifications(bool speed_mod, bool accel_mod);
    
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
