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
    std::vector<PlayerStats> players;
    uint16_t countdown_ms; // timer para la pantalla de stats
};

// para la fase de modificación
struct CarProperties {
    uint16_t speed;
    uint16_t acceleration;
    uint16_t countdown_ms; // timer para la pantalla de mods
};

#endif // GAME_DATA_H