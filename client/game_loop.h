#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include <memory>

#include "../common/messages/snapshot.h"
#include "../common/queue/queue.h"
#include "../common/thread/thread.h"
#include "../common/utils/activeDirections.h"
#include "world.h"


class Game;

/**
 * GameLoop: Thread que ejecuta el loop principal del juego con SDL
 * - Procesa snapshots de la queue del Receiver
 * - Maneja input del usuario
 * - Actualiza el estado del mundo
 * - Renderiza con SDL
 * - Pushea comandos a la queue del Sender
 */
class GameLoop : public Thread {
private:
    Queue<Snapshot>& server_snapshots_q;
    Queue<ActiveDirections>& client_requests_q;
    
    World& world;
    uint8_t player_id;
    
    std::unique_ptr<Game> game;

public:
    /**
     * Constructor
     */
    GameLoop(Queue<Snapshot>& server_snapshots_q,
             Queue<ActiveDirections>& client_requests_q,
             World& world,
             uint8_t player_id);

    /**
     * Loop principal que corre en el thread
     */
    void run() override;
    
    /**
     * Métodos para que otros threads notifiquen eventos
     */
    void on_countdown(uint8_t number);
    void on_race_start();
    void on_checkpoint_crossed(uint8_t checkpoint_id);
    void on_collision(const CollisionData& collision);
    void on_player_died(uint16_t player_id);
    void on_race_end(const RaceResults& results);
    void on_modification_phase(const CarProperties& props);
    void on_game_end(const FinalResults& results);
    
    /**
     * Métodos para enviar comandos (llamados por Game)
     */
    void send_movement(bool up, bool down, bool left, bool right);
    void send_modifications(bool speed, bool health);
    void send_cheat_inmortality();
    void send_cheat_insta_win();
    void send_cheat_insta_lose();

    ~GameLoop() override = default;
};

#endif  // GAME_LOOP_H