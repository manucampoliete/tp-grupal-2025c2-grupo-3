#include "game.h"
#include <chrono>
#include <thread>
#include <algorithm>

#define TARGET_FPS 60
#define FRAME_DURATION_MS (1000 / TARGET_FPS)
#define PLAYER_SPEED 200.0f // píxeles por segundo

Game::Game():
        client_commands_q(),
        response_queues()
        {}

void Game::run() {
    using clock = std::chrono::high_resolution_clock;
    auto last_time = clock::now();

    while (should_keep_running()) {
        auto now = clock::now();
        std::chrono::duration<float> elapsed = now - last_time;
        float delta_time = elapsed.count(); // segundos
        last_time = now;

        std::unique_ptr<Command> cmd;
        while (client_commands_q.try_pop(cmd)) {
            cmd->execute(this);
        }

        // step()

        // broadcast()

        // Mantener FPS constante
        auto frame_time = std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - now).count();
        if (frame_time < FRAME_DURATION_MS) {
            std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_DURATION_MS - frame_time));
        }
    }
}

void Game::stop() {
    Thread::stop();
    client_commands_q.close();      // Receivers cannot push, game cannot try_pop (once it's empty)
    response_queues.close_all();    // Senders cannot pop, game cannot try_push when broadcasting
}

Queue<std::unique_ptr<Command>>& Game::get_client_commands_queue() { return client_commands_q; }

Queue<Snapshot>& Game::get_responses_queue(ClientID client_id) { 
    return response_queues.add_queue(client_id);
}

std::pair<Queue<std::unique_ptr<Command>>&, Queue<Snapshot>&> Game::get_queues(ClientID client_id) {
    return {get_client_commands_queue(), get_responses_queue(client_id)};
}

void Game::add_player(ClientID client_id, const std::string& username, uint8_t car_id) {
    // Inicializar la posición del jugador, por ejemplo en (0,0)
    // positions[client_id] = Vector2D(0.0f, 0.0f);
}

Game::~Game() {}
