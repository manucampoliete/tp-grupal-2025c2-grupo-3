#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <utility>
#include <vector>
#include <atomic>
#include <memory>

#include "../common/commands/move_request.h"
#include "../common/protocol/dummy_client_protocol.h"
#include "../common/queue/queue.h"
#include "../common/socket/socket.h"
#include "../common/utils/vector_2d.h"
#include "../server/types.h"

#include "receiver.h"
#include "sender.h"
#include "world.h"


class Game;
class World;

class Client {
private:
    Socket socket;
    DummyClientProtocol protocol;
    Queue<MoveRequest> client_requests_q;
    Queue<std::vector<std::pair<ClientID, Vector2D>>> server_responses_q;
    Sender sender;
    Receiver receiver;

    World world; 
    Game* game_ptr = nullptr; // puntero a game para poder llamar métodos

    /**
     * Disable copy semantics (not needed and error-prone)
     */
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

public:
    /**
     * Constructor: takes the server hostname and service name (to connect to)
     */
    Client(const std::string& hostname, const std::string& servname);

    /**
     * Runs the client: starts the sender and receiver threads,
     * handles user input and displays server responses.
     */
    void run();
    
    void stop();

    // conecta el cliente con la instancia del juego
    void set_game(Game* game);
    
    // permite al juego acceder al mundo (para dibujar los autos)
    World& get_world();

    // el eventHandler llama a este metodo para enviar movimientos
    void send_movement(bool up, bool down, bool left, bool right);
    
    // el receiver llama a este metodo para actualizar el mundo con datos del servidor
    void update_world(const std::vector<std::pair<ClientID, Vector2D>>& positions);

    // actualiza el estado del world con un nuevo broadcast
    // void update_world(const BroadcastData& data);

    // el servidor asigna y envia un ID único a cada cliente
    // por ahora simulado
    ClientID get_my_id();


    /**
     * Enable move semantics (default implementations are fine)
     */
    Client(Client&&) = default;
    Client& operator=(Client&&) = default;

    /**
     * Destructor
     * Nothing special to do
     */
    ~Client();
};

#endif  // CLIENT_H
