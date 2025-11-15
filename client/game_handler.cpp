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
        receiver(protocol, server_snapshots_q, game_loop),
        game_loop(server_snapshots_q, client_requests_q, protocol, world, player_id),
        running(false) {}


void GameHandler::run() {
    running = true;

    sender.start();
    std::cout << "[GAME_HANDLER] Sender iniciado" << std::endl;

    receiver.start();
    std::cout << "[GAME_HANDLER] Receiver iniciado" << std::endl;

    game_loop.start();
    std::cout << "[GAME_HANDLER] Gameloop iniciado" << std::endl;

    // esperar a que el GameLoop termine (cuando el usuario cierra la ventana)
    game_loop.join();
    std::cout << "[GAME_HANDLER] GameLoop terminado, cerrando otros threads..." << std::endl;

    stop();
}

void GameHandler::stop() {
    std::cout << "[GAME_HANDLER] Deteniendo..." << std::endl;
    running = false;

    client_requests_q.close();
    server_snapshots_q.close();

    sender.stop();
    receiver.stop();
    game_loop.stop();

    sender.join();
    receiver.join();

    std::cout << "[GAME_HANDLER] Stopped." << std::endl;
}


GameHandler::~GameHandler() {
    if (running)
        stop();
}



/*
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
*/