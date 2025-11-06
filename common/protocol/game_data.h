#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <cstdint>
#include <string>
#include <vector>

/*
 * estructuras de datos para el protocolo
 * a chequear segun implementacion
*/

struct Snapshot {
    struct CarSnapshot {
        uint16_t id;        // client/player ID
        uint32_t x;         // x coordinate * 1000
        uint32_t y;         // y coordinate * 1000
        uint16_t angle;     // angle in degrees
        uint16_t speed;     // speed (units?)
        uint8_t type;       // car type (needed?)

        CarSnapshot(uint16_t id, uint32_t x, uint32_t y, uint16_t angle, uint16_t speed, uint8_t type)
            : id(id), x(x), y(y), angle(angle), speed(speed), type(type) {}

        CarSnapshot(const CarSnapshot& other)
            : id(other.id), x(other.x), y(other.y), angle(other.angle), speed(other.speed), type(other.type) {}
    };

    uint16_t countdown;     // remaining race time in milliseconds (or maybe seconds?)
    std::vector<CarSnapshot> cars;

    Snapshot(uint16_t countdown, const std::vector<CarSnapshot>& cars)
        : countdown(countdown), cars(cars) {}

    Snapshot(const Snapshot& other)
        : countdown(other.countdown), cars(other.cars) {}
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
        uint8_t position; // 1, 2, 3 etc
    };

    std::vector<FinalStanding> standings;
    uint16_t winner_id;
    std::string winner_name;
};


// para la fase de modificación
struct CarProperties {
    uint16_t speed;
    uint16_t acceleration;
    uint16_t health;
    uint16_t mass;
    uint16_t handling;
    uint16_t countdown_ms; // timer para la pantalla de mods
};

// datos de colisión
// a chequear desp
struct CollisionData {
    uint16_t player_id; // quién chocó
    float intensity; // 0.0 (leve) a 1.0 (fuerte)
    uint32_t x; // coordenada x del choque * 1000
    uint32_t y; // coordenada y del choque * 1000
};


#endif // GAME_DATA_H