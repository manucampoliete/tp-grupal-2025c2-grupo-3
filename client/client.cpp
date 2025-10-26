#include "client.h"

#include <iostream>

#include <SDL2/SDL.h>

#include "../common/protocol/dummy_client_protocol.h"
#include "../common/socket/socket.h"

Client::Client(const std::string& server_hostname, const std::string& server_servname):
        socket(server_hostname.c_str(), server_servname.c_str()),
        protocol(socket),
        client_requests_q(),
        server_responses_q(),
        sender(protocol, client_requests_q),
        receiver(protocol, server_responses_q) {}

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

Client::~Client() {}
