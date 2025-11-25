#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <cstdint>
#include <string>
#include <vector>

#include <SDL2pp/Rect.hh>


enum class CheatType { NONE, INMORTALITY, INSTA_WIN, INSTA_LOSE, SUPER_SPEED };

// Coordinates (x, y, width, height) in the sprite sheet for each car type
const SDL2pp::Rect CARS[7] = {
        {134, 34, 20, 27},   // Car 0
        {170, 105, 20, 40},  // Car 1
        {170, 186, 20, 38},  // Car 2
        {170, 265, 20, 40},  // Car 3
        {170, 344, 20, 40},  // Car 4
        {170, 425, 20, 40},  // Car 5
        {205, 515, 20, 45},  // Car 6
};

// agregar checkppoint y hints
// agregar health del auto para imprimirlo en la pantalla
struct BroadcastData {
    struct CarState {
        uint8_t id; 
        float x;
        float y; 
        float angle;
        uint8_t type;
        uint8_t health;
        bool onBridge;
    };

    std::vector<CarState> cars;
    uint32_t countdown; 
};


struct RaceResults {
    struct PlayerResult {
        std::string playerName;
        uint32_t raceTimeMs;
        uint32_t totalTimeMs;
    };

    std::vector<PlayerResult> players;
};

struct FinalResults {
    struct FinalStanding {
        uint16_t playerId;
        std::string playerName;
        uint32_t totalTimeMs;
        uint8_t position; 
    };

    std::vector<FinalStanding> standings;
    uint16_t winnerId;
    std::string winnerName;
};


struct CarProperties {
    uint16_t playerId;
    uint16_t speed;
    uint16_t health;
    uint16_t accel;
    uint16_t mass;
};

// datos de colisión
// a chequear desp
struct CollisionData {
    uint16_t playerId;  // quién chocó
    float intensity;     // 0.0 (leve) a 1.0 (fuerte)
    uint32_t x;          // coordenada x del choque * 1000
    uint32_t y;          // coordenada y del choque * 1000
};


// Collision effects (flash)
struct CollisionEffect {
    float x, y;
    float timeAlive;
    float intensity;  // 0-1
    
    CollisionEffect(float x, float y, float intensity)
        : x(x), y(y), timeAlive(0), intensity(intensity) {}
    
    void update(float dt) { timeAlive += dt; }
    
    bool isFinished() const { return timeAlive > 0.3f; }
};

// esplosion particles
struct Particle {
    float x, y;   // position
    float vx, vy; // velocity
    float life;   // remaining life
    SDL_Color color;
    float size;
    
    Particle(float x, float y, float vx, float vy, SDL_Color color, float size = 3.0f)
        : x(x), y(y), vx(vx), vy(vy), life(1.0f), color(color), size(size) {}
    
    void update(float dt) {
        x += vx * dt;
        y += vy * dt;
        vy += 300.0f * dt;  // gravity
        life -= dt * 1.5f;  // they disappear in ~0.66 seconds
        if (life < 0) life = 0;
    }
    
    bool isAlive() const { return life > 0; }
};

// esplosion (a lot of particles)
struct Explosion {
    std::vector<Particle> particles;
    float timeAlive;
    
    Explosion(float x, float y, int particle_count = 30) : timeAlive(0) {
        for (int i = 0; i < particle_count; i++) {
            float angle = (rand() % 360) * 3.14159f / 180.0f;
            float speed = 100.0f + (rand() % 200);
            float vx = cos(angle) * speed;
            float vy = sin(angle) * speed;

            SDL_Color const colors[] = {
                {255, 0, 0, 255},   // red
                {255, 128, 0, 255}, // orange
                {255, 255, 0, 255}  // yellow
            };
            SDL_Color color = colors[rand() % 3];
            
            particles.emplace_back(x, y, vx, vy, color, 2.0f + (rand() % 3));
        }
    }
    
    void update(float dt) {
        timeAlive += dt;
        for (auto& p : particles)
            p.update(dt);
    }
    
    bool isFinished() const {
        return timeAlive > 2.0f || std::all_of(particles.begin(), particles.end(),
                                                  [](const Particle& p) { return !p.isAlive(); });
    }
};

// Smoke particle (for accelerating and post esplosion)
struct SmokeParticle {
    float x, y;
    float vx, vy;
    float life;
    float size;
    uint8_t alpha;
    
    SmokeParticle(float x, float y, float vx, float vy, float size = 8.0f)
        : x(x), y(y), vx(vx), vy(vy), life(1.0f), size(size), alpha(180) {}
    
    void update(float dt) {
        x += vx * dt;
        y += vy * dt;
        vy -= 20.0f * dt;  // goes slowly up
        vx *= 0.95f;       // stops horizontally
        life -= dt * 0.8f;
        size += dt * 15.0f;  // it expands
        alpha = static_cast<uint8_t>(life * 150);
    }
    
    bool is_alive() const { return life > 0; }
};

// Smoke cloud
struct SmokeCloud {
    std::vector<SmokeParticle> particles;
    
    // Smoke behind the car when accelerating
    void addAccelerationSmoke(float x, float y, float carAngle) {
        // Opposite direction 
        float rad = (carAngle - 90) * 3.14159f / 180.0f;
        float backX = x - cos(rad) * 15;
        float backY = y - sin(rad) * 15;
        
        for (int i = 0; i < 2; i++) {
            float spreadX = (rand() % 10 - 5) * 0.5f;
            float spreadY = (rand() % 10 - 5) * 0.5f;
            float vx = -cos(rad) * 30 + spreadX; 
            float vy = -sin(rad) * 30 + spreadY;
            particles.emplace_back(backX, backY, vx, vy, 4.0f);
        }
    }
    
    // Smoke post esplosion (goes up)
    void addExplosionSmoke(float x, float y) {
        for (int i = 0; i < 25; i++) {
            float offsetX = (rand() % 60 - 30);
            float offsetY = (rand() % 60 - 30);
            float vx = (rand() % 20 - 10);
            float vy = -(rand() % 30 + 20);
            particles.emplace_back(x + offsetX, y + offsetY, vx, vy, 10.0f + rand() % 10);
        }
    }
    
    void update(float dt) {
        for (auto& p : particles) p.update(dt);
        
        particles.erase(std::remove_if(particles.begin(), particles.end(),
                      [](const SmokeParticle& p) { return !p.is_alive(); }),
            particles.end());
    }
};

// Brake trail 
// Two lines. Now it doesn't go over the buildings
struct BrakeTrail {
    std::vector<std::pair<float, float>> points;
    float life;
    float alpha;
    static constexpr int MAX_POINTS = 50; // so we dont use too much memory
    
    BrakeTrail(float x, float y) : 
        life(3.0f), alpha(200) {
            points.push_back({x,y});
        }
    
    void extend(float newX, float newY) {
        // only add if its far enough from the last one
        if (!points.empty()) {
            float dx = newX - points.back().first;
            float dy = newY - points.back().second;
            float dist = std::sqrt(dx*dx + dy*dy);
            
            // At least 5px f
            if (dist > 5.0f) {
                points.push_back({newX, newY});
                if (points.size() > MAX_POINTS)
                    points.erase(points.begin());
            }
        }
    }
    
    void update(float dt) {
        life -= dt * 0.3f;  // lasts ~3 seconds
        alpha = static_cast<uint8_t>(std::min(life / 3.0f, 1.0f) * 200);
    }
    
    bool is_alive() const { return life > 0; }
};


#endif  // GAME_DATA_H
