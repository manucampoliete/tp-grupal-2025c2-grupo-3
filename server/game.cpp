#include "game.h"
#include <chrono>
#include <thread>
#include <algorithm>

#define TARGET_FPS 60
#define FRAME_DURATION_MS (1000 / TARGET_FPS)
#define PLAYER_SPEED 200.0f // píxeles por segundo

const float MAP_WIDTH = 4640.0f;
const float MAP_HEIGHT = 4672.0f;
const float PLAYER_SIZE = 20.0f;

void Game::process_request(const MoveRequestWithID& req, float delta_time) {
    Vector2D& pos = positions[req.client_id];

    float distance = PLAYER_SPEED * delta_time;

    if (req.move_request.up)    pos.y -= distance;
    if (req.move_request.down)  pos.y += distance;
    if (req.move_request.left)  pos.x -= distance;
    if (req.move_request.right) pos.x += distance;

    // Mantener dentro de límites (por ejemplo, 0..800x600)
    pos.x = std::clamp(static_cast<float>(pos.x), 0.0f, MAP_WIDTH - PLAYER_SIZE);
    pos.y = std::clamp(static_cast<float>(pos.y), 0.0f, MAP_HEIGHT - PLAYER_SIZE);
}

Game::Game():
        client_commands_q(),
        response_queues(),
        positions() {}

void Game::run() {
    using clock = std::chrono::high_resolution_clock;
    auto last_time = clock::now();

    while (should_keep_running()) {
        auto now = clock::now();
        std::chrono::duration<float> elapsed = now - last_time;
        float delta_time = elapsed.count(); // segundos
        last_time = now;

        MoveRequestWithID req(MoveRequest(false, false, false, false), 0);
        while (client_commands_q.try_pop(req)) {
            process_request(req, delta_time);
            // When command pattern is implemented we'll have commands rather than requests.
            // The client_commands_q will be a Queue<std::unique_ptr<Command>> or similar,
            // and once a command is popped from it we'll do something like:
            // command->execute();
            // Internally, the command will have all the data needed to perform the action,
            // and also a reference to this, or to an object that contains a facade to the game state.
        }

        // Broadcast positions to all clients
        response_queues.broadcast(
            std::vector<std::pair<ClientID, Vector2D>>(positions.begin(), positions.end())
        );

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

Queue<MoveRequestWithID>& Game::get_client_commands_queue() { return client_commands_q; }

ResponseQueuesMonitor& Game::get_response_queues_monitor() { return response_queues; }

Game::~Game() {}
