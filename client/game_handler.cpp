#include "game_handler.h"

#include <iostream>

#include "client.h"
#include "game.h"

#define WORLD_HEIGHT 4672.0f

GameHandler::GameHandler(ClientProtocol& protocol, uint8_t player_id):
        protocol(protocol),
        player_id(player_id),
        world(),
        client_requests_q(),
        server_snapshots_q(),
        sender(protocol, client_requests_q),
        receiver(protocol, server_snapshots_q, *this),
        game(nullptr),
        running(false) {}


void GameHandler::run() {
    running = true;

    sender.start();
    std::cout << "[GAME_HANDLER] Sender iniciado" << std::endl;

    receiver.start();
    std::cout << "[GAME_HANDLER] Receiver iniciado" << std::endl;

    // crear e iniciar el juego (SDL)
    game = std::make_unique<Game>(world, *this, player_id);

    std::cout << "[GAME_HANDLER] Iniciando game loop..." << std::endl;
    game->run();  // blocking hasta que se cierre la ventana

    std::cout << "[GAME_HANDLER] Game loop terminado, cerrando hilos..." << std::endl;
    stop();
}

void GameHandler::stop() {
    std::cout << "[GAME_HANDLER] Deteniendo..." << std::endl;
    running = false;

    client_requests_q.close();
    server_snapshots_q.close();

    sender.stop();
    receiver.stop();
    sender.join();
    receiver.join();

    std::cout << "[GAME_HANDLER] Stopped." << std::endl;
}


void GameHandler::update_world(const Snapshot& snapshot) {
    // convertir Snapshot a BroadcastData (formato del World)
    // desp veo si uso directamente snapshot o si lo dejo asi
    BroadcastData data;

    std::cout << "[GAME_HANDLER] Actualizando world con " << snapshot.cars.size() << " autos"
              << std::endl;

    for (const auto& car_snap: snapshot.cars) {
        BroadcastData::CarState car_state;
        car_state.id = car_snap.id;
        car_state.x = car_snap.x / 1000.0f;  // convertir de uint32_t*1000 a float
        car_state.y =
                WORLD_HEIGHT -
                car_snap.y / 1000.0f;  // traduccion de y entre box2d y sdl2 (tienen el Y al revés)
        car_state.angle = car_snap.angle + 90.0f;  // para que coincida con el angulo 0º de box2d
                                                   // (que el sprite arranque mirando a la derecha)
        car_state.type = car_snap.carId;

        data.cars.push_back(car_state);
    }

    data.countdown = snapshot.countdown;
    world.update(data);
}

void GameHandler::on_countdown(uint8_t number) {
    if (game) {
        game->show_countdown(number);
        
        // sonido de countdown
        if (number <= 3)
            game->get_sound_manager().play_sound("countdown");
        // En GO! desp veo si usar game_start o directamente la musica
    }
}

void GameHandler::on_race_start() {
    if (game) {
        game->start_race();
        // sonido de inicio (diferente al countdown)
        // game->get_sound_manager().play_sound("race_start");
    }
}

void GameHandler::on_checkpoint_crossed(uint8_t checkpoint_id) {
    std::cout << "[GAME_HANDLER] Checkpoint " << (int)checkpoint_id << " crossed!" << std::endl;

    // agregar lo visual
    if (game) {
        game->get_sound_manager().play_sound("checkpoint");
    }
}

void GameHandler::on_collision(const CollisionData& collision) {   
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

void GameHandler::on_player_died(uint16_t dead_player_id) {
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

void GameHandler::on_race_end(const RaceResults& results) {
    if (game) {
        game->show_stats(results);
        
        // sonido de finalización
        game->get_sound_manager().play_sound("race_end");
        
        // pausar musica durante las estadísticas
        game->get_sound_manager().pause_music();
    }
}

void GameHandler::on_modification_phase(const CarProperties& props) {
    if (game)
        game->show_modifications(props);
}

void GameHandler::on_game_end(const FinalResults& results) {   
    std::cout << "[GAME_HANDLER] Game ended! Winner: " << results.winner_name << std::endl;

    if (game) {
        game->get_sound_manager().stop_music();
        // música de victoria/derrota según el resultado??

        // game->show_game_end(results);
    }
}


void GameHandler::send_movement(bool up, bool down, bool left, bool right) {
    ActiveDirections req(up, down, left, right);
    client_requests_q.tryPush(req);
}

void GameHandler::send_modifications(bool speed, bool health) {
    // implementar cuando el protocolo lo soporte
    // por ahora solo log
    std::cout << "[GAME_HANDLER] Modifications: speed=" << speed << ", health=" << health
              << std::endl;
}


void GameHandler::send_cheat_inmortality() {
    protocol.send_inmortality_request();
}

void GameHandler::send_cheat_insta_win() {
    protocol.send_insta_win_request();
}

void GameHandler::send_cheat_insta_lose() {
    protocol.send_insta_lose_request();
}


GameHandler::~GameHandler() {
    if (running)
        stop();
}
