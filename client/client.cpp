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

void Client::on_countdown(uint8_t number) {
      if (game_ptr) game_ptr->show_countdown(number);
  }
  
  void Client::on_race_start() {
      if (game_ptr) game_ptr->start_race();
  }
  
  void Client::on_checkpoint_crossed(uint8_t id) {
      // Opcional: mostrar feedback visual/sonido
  }
  
  void Client::on_collision(CollisionData collision) {
      if (game_ptr) game_ptr->show_collision_effect(collision);
  }
  
  void Client::on_player_died(uint8_t id) {
      if (id == get_my_id() && game_ptr) {
          game_ptr->show_eliminated_screen();
      }
      // Activar animación de explosión para ese auto
      if (game_ptr) game_ptr->trigger_explosion(id);
  }
  
  void Client::on_race_end(RaceResults results) {
      if (game_ptr) game_ptr->show_stats(results);
  }
  
  void Client::on_modification_phase(CarProperties props) {
      if (game_ptr) game_ptr->show_modifications(props);
  }
  
  void Client::on_game_end(FinalResults results) {
      if (game_ptr) game_ptr->show_game_end(results);
  }

*/
