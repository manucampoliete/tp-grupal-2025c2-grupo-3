#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <cstdint>
#include <string>
#include <vector>

/*
 * estructuras de datos para el protocolo
 * a chequear segun implementacion
*/

// para la tabla de estadísticas
struct PlayerStats {
    uint16_t client_id;
    std::string player_name;
    uint16_t race_time_ms;
    uint16_t total_time_ms;
};

struct RaceResults {
    struct PlayerResult {
        std::string player_name;
        uint32_t race_time_ms;
        uint32_t total_time_ms;
    };

    std::vector<PlayerResult> players;
    uint32_t countdown_ms; 
};

// para la fase de modificación
struct CarProperties {
    uint16_t speed;
    uint16_t acceleration;
    uint16_t countdown_ms; // timer para la pantalla de mods
};

#endif // GAME_DATA_H