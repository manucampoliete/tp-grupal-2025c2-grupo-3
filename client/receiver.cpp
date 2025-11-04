#include "receiver.h"
#include "client.h"
#include "../common/commands/move_request.h"

#include <iostream>
#include <syslog.h>

Receiver::Receiver(DummyClientProtocol& protocol, Queue<std::vector<std::pair<ClientID, Vector2D>>>& server_responses_q, Client& client) :
        protocol(protocol),
        server_responses_q(server_responses_q) , client(client) {}

void Receiver::run() {
    while (should_keep_running()) {
        try {
            std::vector<std::pair<ClientID, Vector2D>> resp = protocol.recv_positions_response();
            //server_responses_q.push(resp);

            // le paso los datos al client para que actualice el world
            client.update_world(resp);
        } catch (const std::exception& err) {
            syslog(LOG_INFO, "[Info] Receiver: %s", err.what());
            break;
        }
    }
}


/*

while (should_keep_running()) {
      uint8_t msg_type = protocol.recv_message_type();
      
      switch (msg_type) {
          case MSG_BROADCAST_STATE:
              auto data = protocol.recv_broadcast();
              client.update_world(data);
              break;
          
          case MSG_COUNTDOWN:
              uint8_t number = protocol.recv_countdown();
              client.on_countdown(number);
              break;
          
          case MSG_RACE_START:
              client.on_race_start();
              break;
          
          case MSG_CHECKPOINT_CROSSED:
              uint8_t checkpoint_id = protocol.recv_checkpoint();
              client.on_checkpoint_crossed(checkpoint_id);
              break;
          
          case MSG_COLLISION:
              auto collision = protocol.recv_collision();
              client.on_collision(collision);
              break;
          
          case MSG_PLAYER_DIED:
              uint8_t dead_id = protocol.recv_player_died();
              client.on_player_died(dead_id);
              break;
          
          case MSG_RACE_END:
              auto results = protocol.recv_race_results();
              client.on_race_end(results);
              break;
          
          case MSG_MODIFICATION_PHASE:
              auto props = protocol.recv_car_properties();
              client.on_modification_phase(props);
              break;
          
          case MSG_GAME_END:
              auto final_results = protocol.recv_final_results();
              client.on_game_end(final_results);
              break;
      }
  }

*/