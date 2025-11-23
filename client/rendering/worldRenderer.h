#ifndef WORLD_RENDERER_H
#define WORLD_RENDERER_H

#include <cstdint>
#include <map>
#include <vector>
#include <memory>

#include <SDL.h>
#include <SDL2pp/SDL2pp.hh>
#include <SDL2pp/Rect.hh>
#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>

#include "../utils/gameData.h"
#include "../gameHandling/world.h"
#include "bridgeRenderer.h"

using namespace SDL2pp;  // NOLINT


class WorldRenderer {
private:
    Renderer& renderer;
    Texture& mapTexture;
    Texture& carSprites;

    std::unique_ptr<Texture> bridgeTexture;
    std::unique_ptr<BridgeRenderer> bridgeRenderer;

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
    
    void renderMapCamera();
    void renderCarsUnderBridge();
    void renderBridges();
    void renderCarsOnBridge();
    void renderCar(const BroadcastData::CarState& carState);

    void renderSmoke();
    void renderBrakeTrails();
    void renderExplosions();
    void renderCollisionEffects();

public:
    WorldRenderer(Renderer& renderer, Texture& mapTexture, Texture& carSprites, World& world,
                  uint8_t playerId);
    
    // Load bridge texture for current map
    void loadBridgeTexture(const std::string& path);
    
    // Check if bridges are loaded for current map
    bool hasBridges() const { return bridgeRenderer != nullptr; }
    
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
