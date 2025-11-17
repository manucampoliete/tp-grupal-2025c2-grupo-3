#include "game_loop.h"

#include <iostream>
#include <utility>

#include "game.h"

#define WORLD_HEIGHT 4672.0f


GameLoop::GameLoop(Queue<Snapshot>& server_snapshots_q, Queue<ActiveDirections>& client_requests_q,
                   ClientProtocol& protocol, World& world, uint8_t player_id):
        server_snapshots_q(server_snapshots_q),
        client_requests_q(client_requests_q),
        protocol(protocol),
        world(world),
        player_id(player_id),
        game(nullptr) {}

void GameLoop::run() {
    game = std::make_unique<Game>(world, *this, player_id);

    std::cout << "[GAME_LOOP] Iniciando SDL game loop..." << std::endl;

    const int FRAME_RATE = 60;
    const float FRAME_TIME_MS = 1000.0f / FRAME_RATE;

    float t1 = SDL_GetTicks();
    bool is_running = true;

    while (is_running && shouldKeepRunning()) {
        // consumir todos pero aplicar solo el ult
        Snapshot latest_snapshot;
        bool has_snapshot = false;

        {
            Snapshot temp;
            while (server_snapshots_q.tryPop(temp)) {
                latest_snapshot = std::move(temp);
                has_snapshot = true;
            }
        }

        // !! solo aplicar el ultimo snapshot recibido
        if (has_snapshot) {
            BroadcastData data;

            for (const auto& car_snap: latest_snapshot.cars) {
                BroadcastData::CarState car_state;
                car_state.id = car_snap.id;
                car_state.x = car_snap.x / 1000.0f;
                car_state.y = WORLD_HEIGHT - car_snap.y / 1000.0f;
                car_state.angle = car_snap.angle + 90.0f;
                car_state.type = car_snap.carId;

                data.cars.push_back(car_state);
            }

            data.countdown = latest_snapshot.countdown;
            world.update(data);
        }

        // procesar frame (input + update + render)
        is_running = game->process_frame(FRAME_TIME_MS);

        // sincronizar con frame rate
        float t2 = SDL_GetTicks();
        float rest = FRAME_TIME_MS - (t2 - t1);

        if (rest < 0) {
            float behind = -rest;
            rest = FRAME_TIME_MS - fmod(behind, FRAME_TIME_MS);
            float lost = behind + rest;
            t1 += lost;
        }

        SDL_Delay(static_cast<Uint32>(rest));
        t1 += FRAME_TIME_MS;
    }

    std::cout << "[GAME_LOOP] Thread detenido" << std::endl;
}


void GameLoop::onCountdown(uint8_t number) {
    if (game)
        game->show_countdown(number);
}

void GameLoop::onRaceStart() {
    if (game)
        game->start_race();
}


// CORREGIR
void GameLoop::onCheckpointCrossed(uint8_t checkpointId) {
    std::cout << "[GAME_HANDLER] Checkpoint " << (int)checkpointId << " crossed!" << std::endl;

    // a chequear!

    // agregar lo visual
    if (game)
        game->get_sound_manager().play_sound("checkpoint");
}


void GameLoop::onCollision(const CollisionData& collision) {
    std::cout << "[GAME_LOOP] Colisión detectada, intensity: " << collision.intensity << std::endl;
    if (!game)
        return;

    // coordenadas del servidor mm a metros
    float world_x = collision.x / 1000.0f;
    float world_y = WORLD_HEIGHT - (collision.y / 1000.0f);
    game->on_collision(world_x, world_y, collision.intensity);
}

void GameLoop::onPlayerDied(uint16_t deadPlayerId) {
    if (game)
        game->on_player_died(deadPlayerId);
}

void GameLoop::onRaceEnd(const RaceResults& results) {
    if (game)
        game->show_stats(results);
}

void GameLoop::onModificationPhase(const CarProperties& props) {
    if (game)
        game->show_modifications(props);
}

void GameLoop::onGameEnd(const FinalResults& results) {
    if (game)
        game->show_final_results(results);
}


void GameLoop::send_movement(bool up, bool down, bool left, bool right) {
    ActiveDirections req(up, down, left, right);
    client_requests_q.tryPush(req);
}

void GameLoop::send_modifications(bool speed, bool health) {
    std::cout << "[GAME_HANDLER] Modifications: speed=" << speed << ", health=" << health
              << std::endl;
    protocol.sendModifications(speed, health);
}

void GameLoop::send_cheat_inmortality() {
    protocol.sendInmortalityRequest();
    if (game)
        game->show_cheat_notification(CheatType::INMORTALITY);
}

void GameLoop::send_cheat_insta_win() {
    protocol.sendInstaWinRequest();
    if (game)
        game->show_cheat_notification(CheatType::INSTA_WIN);
}

void GameLoop::send_cheat_insta_lose() {
    protocol.sendInstaLoseRequest();
    if (game)
        game->show_cheat_notification(CheatType::INSTA_LOSE);
}
