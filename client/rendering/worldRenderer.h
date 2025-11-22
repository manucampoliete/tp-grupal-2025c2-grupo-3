#ifndef WORLD_RENDERER_H
#define WORLD_RENDERER_H

#include <cstdint>
#include <map>
#include <vector>

#include <SDL.h>
#include <SDL2pp/Rect.hh>
#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>

#include "../../common/messages/gameData.h"

#include "../gameHandling/world.h"

using namespace SDL2pp;  // NOLINT


class WorldRenderer {
private:
    Renderer& renderer;
    Texture& mapTexture;
    Texture& carSprites;
    World& world;
    uint8_t playerId;

    Rect camera;
    float scaleFactor;
    const float REFERENCE_HEIGHT = 600.0f;

    std::vector<Explosion> explosions;
    std::vector<CollisionEffect> collisionEffects;

    SmokeCloud smokeCloud;
    std::vector<BrakeTrail> brakeTrails;
    BrakeTrail* currentBrakeTrail = nullptr;
    
    bool isAccelerating = false;
    bool isBraking = false;
    
    void renderSmoke();
    void renderBrakeTrails();

    void renderMapCamera();
    void renderAllCars();
    void renderExplosions();
    void renderCollisionEffects();

public:
    WorldRenderer(Renderer& renderer, Texture& mapTexture, Texture& carSprites, World& world,
                  uint8_t playerId);
    
    // Render the world (map + cars) with camera and scaling
    void render();
    void updateLayout(int windowWidth, int windowHeight);
    void updateCamera(float playerX, float playerY);
    void updateEffects(float dt);

    const Rect& getCamera() const { return camera; }
    float getScaleFactor() const { return scaleFactor; }

    void addExplosion(float x, float y, int particleCount = 30);
    void addCollisionEffect(float x, float y, float intensity);

    void setAccelerating(bool accelerating);
    void setBraking(bool braking);
    void addExplosionSmoke(float x, float y);
};

#endif  // WORLD_RENDERER_H
