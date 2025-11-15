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
    uint8_t recv_message_type();

    void send_move(const ActiveDirections& request);
    void send_modifications(bool speed_mod, bool health_mod);

    Snapshot recv_snapshot();
    uint8_t recv_countdown();
    uint8_t recv_checkpoint();
    CollisionData recv_collision();
    uint16_t recv_player_died();
    RaceResults recv_race_results();
    CarProperties recv_car_properties();
    FinalResults recv_final_results();

    // CHEATS
    void send_inmortality_request();
    void send_insta_win_request();
    void send_insta_lose_request();
};

#endif  // CLIENT_PROTOCOL_H
