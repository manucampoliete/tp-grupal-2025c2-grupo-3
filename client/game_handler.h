#ifndef GAME_HANDLER_H
#define GAME_HANDLER_H

#include <atomic>
#include <memory>

#include "../common/messages/game_data.h"
#include "../common/messages/snapshot.h"
#include "../common/queue/queue.h"
#include "../common/utils/activeDirections.h"
#include "../protocol/client_protocol.h"

#include "receiver.h"
#include "sender.h"
#include "world.h"


class Game;
class Client;


// lo que antes tenia en client ahora lo apso a game handler
/**
 * GameHandler: coordinar la lógica del juego en el cliente
 * - Maneja el World (estado del juego)
 * - Maneja Game (SDL)
 * - Maneja Sender/Receiver (comunicación con servidor)
 * - Procesa eventos del servidor y los traduce a acciones en el Game
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

    // juego (SDL)
    std::unique_ptr<Game> game;

    // control de ejecución
    std::atomic<bool> running;

public:
    // player_id: id asignado al jugador (por ahora hardcodeado)
    GameHandler(ClientProtocol& protocol, uint8_t player_id);

    // ejecuta el juego completo:
    // inicia hilos, lanza el game loop de SDL y procesa eventos del servidor
    void run();

    // detiene el juego y los hilos
    void stop();


    // actualiza el estado del mundo con un nuevo snapshot del servidor
    void update_world(const Snapshot& snapshot);

    // muestra countdown en pantalla (3, 2, 1, GO!)
    void on_countdown(uint8_t number);

    // inicia la carrera
    void on_race_start();

    // el jugador cruzó un checkpoint
    // PARA MAS ADELANTE!
    void on_checkpoint_crossed(uint8_t checkpoint_id);

    // Notificación de colisión
    // PARA MAS ADELANTE!
    void on_collision(const CollisionData& collision);

    // Un jugador murió
    // PARA MAS ADELANTE!
    void on_player_died(uint16_t player_id);

    // Terminó la carrera, mostrar resultados
    void on_race_end(const RaceResults& results);

    // fase de modificación de auto
    void on_modification_phase(const CarProperties& props);

    // partida completa terminada, mostrar ganador
    void on_game_end(const FinalResults& results);


    // envía un input de movimiento al servidor
    void send_movement(bool up, bool down, bool left, bool right);

    // envía modificaciones del auto al servidor
    void send_modifications(bool speed, bool health);

    World& get_world() { return world; }
    uint8_t get_player_id() const { return player_id; }


    ~GameHandler();
};

#endif  // GAME_HANDLER_H
