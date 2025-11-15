#include "game_loop.h"
#include "game.h"

#include <iostream>

#define WORLD_HEIGHT 4672.0f

GameLoop::GameLoop(Queue<Snapshot>& server_snapshots_q, Queue<ActiveDirections>& client_requests_q, ClientProtocol& protocol, World& world, uint8_t player_id) :
    server_snapshots_q(server_snapshots_q),
    client_requests_q(client_requests_q),
    protocol(protocol),
    world(world),
    player_id(player_id),
    game(nullptr) {}

void GameLoop::run() {
    std::cout << "[GAME_LOOP] Thread iniciado" << std::endl;
    
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
            
            for (const auto& car_snap : latest_snapshot.cars) {
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



void GameLoop::on_countdown(uint8_t number) {
    if (game) {
        game->show_countdown(number);
        
        // sonido de countdown
        if (number <= 3)
            game->get_sound_manager().play_sound("countdown");
        // En GO! desp veo si usar game_start o directamente la musica
    }
}

void GameLoop::on_race_start() {
    if (game) {
        game->start_race();
        // sonido de inicio (diferente al countdown)
        // game->get_sound_manager().play_sound("race_start");
    }
}

void GameLoop::on_checkpoint_crossed(uint8_t checkpoint_id) {
    std::cout << "[GAME_HANDLER] Checkpoint " << (int)checkpoint_id << " crossed!" << std::endl;

    // agregar lo visual
    if (game) {
        game->get_sound_manager().play_sound("checkpoint");
    }
}

void GameLoop::on_collision(const CollisionData& collision) {   
    std::cout << "[GAME_HANDLER] Colisión detected, intensity: " << collision.intensity
              << std::endl;
    
    if (game) {
    
    //     game->show_collision_effect(collision);

        // VOL SEGÚN INTENSIDAD
        int volume = static_cast<int>(collision.intensity * MIX_MAX_VOLUME);
        game->get_sound_manager().play_sound("collision", volume);
        
        // VOL SEGÚN DISTANCIA
        // calcular distancia del jugador a la colisión
        auto cars = world.getCars();
        if (cars.count(player_id)) {
            const auto& my_car = cars.at(player_id);
            
            float dx = my_car.x - (collision.x / 1000.0f);
            float dy = my_car.y - (collision.y / 1000.0f);
            float distance = std::sqrt(dx * dx + dy * dy);
            
            // reproducir con volumen modulado por distancia
            game->get_sound_manager().play_sound_with_distance("collision", distance, 500.0f);
        }
    }
}

void GameLoop::on_player_died(uint16_t dead_player_id) {
    std::cout << "[GAME_HANDLER] Player " << dead_player_id << " has died." << std::endl;

    if (game) {
        game->get_sound_manager().play_sound("explosion");
        
        if (dead_player_id == player_id) {
            std::cout << "[GAME_HANDLER] You died!" << std::endl;
            // game->show_eliminated_screen();
            game->get_sound_manager().pause_music();
        } else {
            // activar animación de explosión para ese jugador
            // game->trigger_explosion(dead_player_id);
            
        }
    }
}

void GameLoop::on_race_end(const RaceResults& results) {
    if (game) {
        game->show_stats(results);
        
        // sonido de finalización
        game->get_sound_manager().play_sound("race_end");
        
        // pausar musica durante las estadísticas
        game->get_sound_manager().pause_music();
    }
}

void GameLoop::on_modification_phase(const CarProperties& props) {
    if (game)
        game->show_modifications(props);
}

void GameLoop::on_game_end(const FinalResults& results) {   
    std::cout << "[GAME_HANDLER] Game ended! Winner: " << results.winner_name << std::endl;

    if (game) {
        game->get_sound_manager().stop_music();
        // música de victoria/derrota según el resultado??

        // game->show_game_end(results);
    }
}


void GameLoop::send_movement(bool up, bool down, bool left, bool right) {
    ActiveDirections req(up, down, left, right);
    client_requests_q.tryPush(req);
}

void GameLoop::send_modifications(bool speed, bool health) {
    // implementar cuando el protocolo lo soporte
    // por ahora solo log
    std::cout << "[GAME_HANDLER] Modifications: speed=" << speed << ", health=" << health
              << std::endl;
}


void GameLoop::send_cheat_inmortality() {
    protocol.send_inmortality_request();
    if (game)
        game->show_cheat_notification(CheatType::INMORTALITY);
}

void GameLoop::send_cheat_insta_win() {
    protocol.send_insta_win_request();
    if (game)
        game->show_cheat_notification(CheatType::INSTA_WIN);
}

void GameLoop::send_cheat_insta_lose() {
    protocol.send_insta_lose_request();
    if (game)
        game->show_cheat_notification(CheatType::INSTA_LOSE);
}





