#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include <cstdint>
#include <string>
#include <vector>

#include "../../common/messages/game_data.h"
#include "../../common/messages/snapshot.h"
#include "../../common/protocol/recvProtocol.h"
#include "../../common/protocol/sendProtocol.h"
#include "../../common/socket/socket.h"
#include "../../common/types/types.h"
#include "../../common/utils/activeDirections.h"
#include "../../common/utils/carinfo.h"

class ClientProtocol: public SendProtocol, public RecvProtocol {
public:
    explicit ClientProtocol(Socket& socket);


    /**
     * HANDSHAKE
     * TODO: (Manu) Unificar ambos métodos en uno solo?
     */
    ClientID recv_client_id();
    std::vector<CarInfo> recv_initial_info();


    /**
     * LOBBY
     */
    uint16_t send_create(const std::string& username, uint8_t car_id);
    bool send_join(uint16_t match_id, const std::string& username, uint8_t car_id);
    void send_start();
    void recv_start_signal();


    /**
     * GAME
     */
    // Lee el tipo de mensaje del servidor y devuelve el código
    uint8_t recv_message_type();

    // Envía un input de movimiento al servidor
    void send_move(const ActiveDirections& request);

    // Recibe un snapshot del estado del juego (ahora usando snapshot.h)
    Snapshot recv_snapshot();

    // Envía modificaciones del auto al servidor
    void send_modifications(bool speed_mod, bool health_mod);

    // Recibe número de countdown (3, 2, 1, 0=GO)
    uint8_t recv_countdown();

    // Recibe ID de checkpoint cruzado
    uint8_t recv_checkpoint();

    // Recibe datos de colisión
    CollisionData recv_collision();

    // Recibe ID del jugador que murió
    uint16_t recv_player_died();

    // Recibe resultados de la carrera
    RaceResults recv_race_results();

    // Recibe propiedades del auto para modificación
    CarProperties recv_car_properties();

    // Recibe resultados finales de la partida
    FinalResults recv_final_results();

    // Cheats!
    void send_inmortality_request();
    void send_insta_win_request();
    void send_insta_lose_request();
};

#endif  // CLIENT_PROTOCOL_H
