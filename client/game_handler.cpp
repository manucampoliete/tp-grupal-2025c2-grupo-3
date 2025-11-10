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
    if (game)
        game->show_countdown(number);
}

void GameHandler::on_race_start() {
    if (game)
        game->start_race();
}

void GameHandler::on_checkpoint_crossed(uint8_t checkpoint_id) {
    // agregar lo visual y el sonido!!
    std::cout << "[GAME_HANDLER] Checkpoint " << (int)checkpoint_id << " crossed!" << std::endl;
}

void GameHandler::on_collision(const CollisionData& collision) {
    // implementar cuando Game tenga el método
    // if (game)
    //     game->show_collision_effect(collision);
    std::cout << "[GAME_HANDLER] Colisión detected, intensity: " << collision.intensity
              << std::endl;
}

void GameHandler::on_player_died(uint16_t dead_player_id) {
    if (game) {
        if (dead_player_id == player_id) {
            // implementar cuando Game tenga el método
            // game->show_eliminated_screen();
            std::cout << "[GAME_HANDLER] You died!" << std::endl;
        } else {
            // activar animación de explosión para ese jugador
            // game->trigger_explosion(dead_player_id);
            std::cout << "[GAME_HANDLER] Player " << dead_player_id << " has died." << std::endl;
        }
    }
}

void GameHandler::on_race_end(const RaceResults& results) {
    if (game)
        game->show_stats(results);
}

void GameHandler::on_modification_phase(const CarProperties& props) {
    if (game)
        game->show_modifications(props);
}

void GameHandler::on_game_end(const FinalResults& results) {
    // implementar cuando Game tenga el método
    // if (game)
    //     game->show_game_end(results);
    std::cout << "[GAME_HANDLER] Game ended! Winner: " << results.winner_name << std::endl;
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

GameHandler::~GameHandler() {
    if (running)
        stop();
}
