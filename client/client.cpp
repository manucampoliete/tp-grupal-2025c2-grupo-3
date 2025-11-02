#include "client.h"
#include "game.h"
#include "../common/protocol/dummy_client_protocol.h"
#include "../common/socket/socket.h"

#include <iostream>
#include <utility>
#include <sys/socket.h>


Client::Client(const std::string& hostname, const std::string& servname):
        socket(hostname.c_str(), servname.c_str()),
        protocol(socket),
        client_requests_q(),
        server_responses_q(),
        sender(protocol, client_requests_q),
        receiver(protocol, server_responses_q, *this) {}

void Client::run() {
    sender.start();
    receiver.start();
}

void Client::stop() {
    try {
        // cierra la comunicación y detiene los hilos
        client_requests_q.close();
        server_responses_q.close();
        sender.stop();
        receiver.stop();
        sender.join();
        receiver.join();
        socket.shutdown(SHUT_RDWR);
        socket.close();
    } catch (const std::exception& e) {
        std::cerr << "Error al detener el cliente: " << e.what() << std::endl;
    }
    std::cout << "[CLIENT] Cliente detenido." << std::endl;
}


void Client::set_game(Game* game) {
    this->game_ptr = game;
}

World& Client::get_world() {
    return world;
}


void Client::send_movement(bool up, bool down, bool left, bool right) {
    MoveRequest req(up, down, left, right);
    client_requests_q.try_push(req); // try_push para no bloquear el hilo de juego
}

void Client::send_modifications(bool speed, bool accel) {
    try {
        // Esta es la línea que faltaba:
        protocol.send_modifications(speed, accel);
    } catch (const std::exception& e) {
        std::cerr << "Error al enviar modificaciones: " << e.what() << std::endl;
    }
}

void Client::update_world(const std::vector<std::pair<ClientID, Vector2D>>& positions) {
    // cambio la informacion a la estructura del broadcast
    // despues vemos si lo pasamos directamente asi o si dejamos esta transformacion
    BroadcastData data;
    for (const auto& p : positions) {
        BroadcastData::CarState car_state;
        car_state.id = p.first;
        car_state.x = p.second.x;
        car_state.y = p.second.y;

        // el protocolo debe enviar ángulo
        // el tipo de auto ya lo tendria asignado el cliente desde que lo elige en el lobby
        // por ahora, los dejo en 0

        car_state.angle = 0.0f; 
        car_state.type = 0;
        data.cars.push_back(car_state);
    }

    // el protocolo deberá enviar el tiempo restante
    data.countdown = 60000; // 1 minuto como ejemplo
    
    world.update(data);
}

void Client::show_stats_screen(const RaceResults& results) {
    if (game_ptr) {
        game_ptr->show_stats(results);
    }
}

void Client::show_mod_screen(const CarProperties& props) {
    if (game_ptr) {
        game_ptr->show_modifications(props);
    }
}




ClientID Client::get_my_id() {
    // TEMPORAL
    // el server va a mandar un mensaje con el id asignado al cliente
    return 0;
}

Client::~Client() {
    stop();
}




/*


int Client::run() {
    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error al inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Crear ventana
    SDL_Window* window = SDL_CreateWindow("Cliente - Visualización SDL", SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);

    if (!window) {
        printf("Error al crear la ventana: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Crear renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Error al crear el renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    sender.start();
    receiver.start();

    std::vector<std::pair<ClientID, Vector2D>> positions;

    bool running = true;
    while (running) {
        try {
            SDL_PumpEvents();  // Actualiza el estado del teclado

            const Uint8* state = SDL_GetKeyboardState(NULL);

            if (state[SDL_SCANCODE_ESCAPE])
                running = false;

            // Enviar movimiento
            MoveRequest req(state[SDL_SCANCODE_W], state[SDL_SCANCODE_S], state[SDL_SCANCODE_A],
                            state[SDL_SCANCODE_D]);
            client_requests_q.try_push(req);

            // Recibir posiciones del servidor
            std::vector<std::pair<ClientID, Vector2D>> resp;
            if (server_responses_q.try_pop(resp)) {
                positions = resp;
            }

            // --- DIBUJADO ---
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // negro de fondo
            SDL_RenderClear(renderer);

            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // verde para jugadores

            for (const auto& p: positions) {
                int x = static_cast<int>(p.second.x);
                int y = static_cast<int>(p.second.y);

                SDL_Rect rect;
                rect.x = x;
                rect.y = y;
                rect.w = 20;
                rect.h = 20;
                SDL_RenderFillRect(renderer, &rect);
            }

            SDL_RenderPresent(renderer);

            SDL_Delay(17);  // ~60 FPS
        } catch (const std::exception& e) {
            std::cerr << "[Client] Error: " << e.what() << std::endl;
            running = false;
        }
    }

    // Liberar recursos
    sender.stop();
    receiver.stop();
    sender.join();
    receiver.join();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}


*/
