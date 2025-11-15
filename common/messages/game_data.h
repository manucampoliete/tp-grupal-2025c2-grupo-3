#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <cstdint>
#include <string>
#include <vector>

#include <SDL2pp/Rect.hh>


/*
 * estructuras de datos para el protocolo
 * a chequear segun implementacion
 */

enum class CheatType { NONE, INMORTALITY, INSTA_WIN, INSTA_LOSE };

// coordenadas (x, y, ancho, alto) para cada auto
const SDL2pp::Rect CARS[7] = {
        {134, 34, 20, 27},   // auto 1
        {170, 105, 20, 40},  // auto 2
        {170, 186, 20, 38},  // auto 3
        {170, 265, 20, 40},  // auto 4
        {170, 344, 20, 40},  // auto 5
        {170, 425, 20, 40},  // auto 6
        {205, 515, 20, 45},  // auto 7
};


// la información que llega del servidor
// - posición, rotación y tipo de auto de cada jugador
// - tiempo restante de la carrera
struct BroadcastData {
    struct CarState {
        uint8_t id;    // id del jugador
        float x;       // posición x
        float y;       // posición y
        float angle;   // ángulo en grados
        uint8_t type;  // tipo de auto
    };

    std::vector<CarState> cars;
    uint32_t countdown;  // tiempo restante de la carrera en milisegundos
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

struct FinalResults {
    struct FinalStanding {
        uint16_t player_id;
        std::string player_name;
        uint32_t total_time_ms;
        uint8_t position;  // 1, 2, 3 etc
    };

    std::vector<FinalStanding> standings;
    uint16_t winner_id;
    std::string winner_name;
};


// para la fase de modificación
struct CarProperties {
    uint16_t speed;
    uint16_t health;
    uint16_t countdown_ms;  // timer para la pantalla de mods
};

// datos de colisión
// a chequear desp
struct CollisionData {
    uint16_t player_id;  // quién chocó
    float intensity;     // 0.0 (leve) a 1.0 (fuerte)
    uint32_t x;          // coordenada x del choque * 1000
    uint32_t y;          // coordenada y del choque * 1000
};

#endif  // GAME_DATA_H
