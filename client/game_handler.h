#ifndef GAME_HANDLER_H
#define GAME_HANDLER_H

#include <atomic>
#include <memory>

#include "../common/messages/game_data.h"
#include "../common/messages/snapshot.h"
#include "../common/queue/queue.h"
#include "../common/utils/activeDirections.h"
#include "../protocol/client_protocol.h"

#include "game_loop.h"
#include "receiver.h"
#include "sender.h"
#include "world.h"


/**
 * GameHandler: coordinar la lógica del juego en el cliente
 * - Maneja el World (estado del juego)
 * - Maneja Sender, Receiver, GameLoop
 * - Procesa eventos del servidor
 */
class GameHandler {
private:
    ClientProtocol& protocol;
    uint8_t player_id;

    // estado del juego
    World world;

    Queue<ActiveDirections> client_requests_q;
    Queue<Snapshot> server_snapshots_q;

    Sender sender;
    Receiver receiver;
    GameLoop game_loop;

    // control de ejecución
    std::atomic<bool> running;

public:
    // player_id: id asignado al jugador (por ahora hardcodeado)
    GameHandler(ClientProtocol& protocol, uint8_t player_id);

    // ejecuta el juego completo:
    // inicia los 3 hilos
    void run();

    // detiene el juego y los hilos
    void stop();


    // actualiza el estado del mundo con un nuevo snapshot del servidor
 //   void update_world(const Snapshot& snapshot);

    ~GameHandler();
};

#endif  // GAME_HANDLER_H
