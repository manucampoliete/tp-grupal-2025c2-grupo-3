#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include <memory>

#include "../common/messages/game_data.h"
#include "../common/messages/snapshot.h"
#include "../common/queue/queue.h"
#include "../common/thread/thread.h"
#include "../common/utils/activeDirections.h"
#include "protocol/clientProtocol.h"

#include "world.h"


class Game;

/**
 * GameLoop: Thread que ejecuta el loop principal del juego con SDL
 * - Procesa snapshots de la queue del Receiver
 * - Maneja input del usuario
 * - Pushea comandos a la queue del Sender
 */
class GameLoop: public Thread {
private:
    Queue<Snapshot>& server_snapshots_q;
    Queue<ActiveDirections>& client_requests_q;
    ClientProtocol& protocol;

    World& world;
    uint8_t player_id;

    std::unique_ptr<Game> game;

public:
    /**
     * Constructor
     */
    GameLoop(Queue<Snapshot>& server_snapshots_q, Queue<ActiveDirections>& client_requests_q,
             ClientProtocol& protocol, World& world, uint8_t player_id);

    /**
     * Loop principal que corre en el thread
     */
    void run() override;

    /**
     * Métodos para que otros threads notifiquen eventos
     */
    void onCountdown(uint8_t number);
    void onRaceStart();
    void onCheckpointCrossed(uint8_t checkpointId);
    void onCollision(const CollisionData& collision);
    void onPlayerDied(uint16_t deadPlayerId);
    void onRaceEnd(const RaceResults& results);
    void onModificationPhase(const CarProperties& props);
    void onGameEnd(const FinalResults& results);

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
