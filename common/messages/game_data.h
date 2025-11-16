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


// Efecto de colisión (flash temporal)
struct CollisionEffect {
    float x, y;
    float time_alive;
    float intensity;  // 0-1
    
    CollisionEffect(float x, float y, float intensity)
        : x(x), y(y), time_alive(0), intensity(intensity) {}
    
    void update(float dt) { time_alive += dt; }
    
    bool is_finished() const { return time_alive > 0.3f; }
};

// particula para explosiones
struct Particle {
    float x, y; // posición
    float vx, vy; // velocidad
    float life; // vida restante (0-1)
    SDL_Color color;
    float size;
    
    Particle(float x, float y, float vx, float vy, SDL_Color color, float size = 3.0f)
        : x(x), y(y), vx(vx), vy(vy), life(1.0f), color(color), size(size) {}
    
    void update(float dt) {
        x += vx * dt;
        y += vy * dt;
        vy += 300.0f * dt;  // gravedad
        life -= dt * 1.5f;  // se desvanecen en ~0.66 segundos
        if (life < 0) life = 0;
    }
    
    bool is_alive() const { return life > 0; }
};

// explosión (conjunto de partículas)
struct Explosion {
    std::vector<Particle> particles;
    float time_alive;
    
    Explosion(float x, float y, int particle_count = 30) : time_alive(0) {
        for (int i = 0; i < particle_count; i++) {
            float angle = (rand() % 360) * 3.14159f / 180.0f;
            float speed = 100.0f + (rand() % 200);
            float vx = cos(angle) * speed;
            float vy = sin(angle) * speed;
            
            // Colores de fuego: rojo, naranja, amarillo
            SDL_Color const colors[] = {
                {255, 0, 0, 255}, // rojo
                {255, 128, 0, 255}, // naranja
                {255, 255, 0, 255} // amarillo
            };
            SDL_Color color = colors[rand() % 3];
            
            particles.emplace_back(x, y, vx, vy, color, 2.0f + (rand() % 3));
        }
    }
    
    void update(float dt) {
        time_alive += dt;
        for (auto& p : particles)
            p.update(dt);
    }
    
    bool is_finished() const {
        return time_alive > 2.0f || std::all_of(particles.begin(), particles.end(),
                                                  [](const Particle& p) { return !p.is_alive(); });
    }
};


#endif  // GAME_DATA_H
