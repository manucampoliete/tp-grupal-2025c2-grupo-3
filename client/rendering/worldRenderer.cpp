#include "worldRenderer.h"
#include <iostream>
#include <cmath>


WorldRenderer::WorldRenderer(Renderer& renderer, Texture& mapTexture, Texture& carSprites,
                             World& world, uint8_t playerId):
        renderer(renderer),
        mapTexture(mapTexture),
        carSprites(carSprites),
        bridgeTexture(nullptr),
        bridgeRenderer(nullptr),
        checkpointRenderer(renderer),
        world(world),
        playerId(playerId),
        camera(0, 0, 800, 600),
        scaleFactor(1.0f) {}


void WorldRenderer::loadBridgeTexture(const std::string& path) {
    try {
        SDL2pp::Surface surface(path);
        bridgeTexture = std::make_unique<Texture>(renderer, surface);
        bridgeTexture->SetBlendMode(SDL_BLENDMODE_BLEND);
        
        bridgeRenderer = std::make_unique<BridgeRenderer>(renderer, *bridgeTexture);
    } catch (const std::exception& e) {
        std::cerr << "[WORLD_RENDERER] Error loading bridge texture: " << e.what() << std::endl;
        bridgeTexture = nullptr;
        bridgeRenderer = nullptr;
    }
}

void WorldRenderer::updateLayout(int windowWidth, int windowHeight) {
    // Update scale factor
    scaleFactor = (float)windowHeight / REFERENCE_HEIGHT;

    // Update camera to match window size
    // but inversely scaled (if window is 2x, camera sees half)
    camera.w = static_cast<int>(windowWidth / scaleFactor);
    camera.h = static_cast<int>(windowHeight / scaleFactor);
}


void WorldRenderer::updateCamera(float playerX, float playerY) {
    camera.x = static_cast<int>(playerX - (camera.w / 2));
    camera.y = static_cast<int>(playerY - (camera.h / 2));

    // Avoid camera going out of map bounds
    if (camera.x < 0) camera.x = 0;
    if (camera.y < 0) camera.y = 0;
    if (camera.x > mapTexture.GetWidth() - camera.w)
        camera.x = mapTexture.GetWidth() - camera.w;
    if (camera.y > mapTexture.GetHeight() - camera.h)
        camera.y = mapTexture.GetHeight() - camera.h;
}


void WorldRenderer::updateEffects(float dt) {
    // Update explosions
    for (auto& explosion: explosions) explosion.update(dt);
    explosions.erase(std::remove_if(explosions.begin(), explosions.end(),
                                    [](const Explosion& e) { return e.isFinished(); }),
                     explosions.end());

    // Update collisions
    for (auto& collision: collisionEffects) collision.update(dt);
    collisionEffects.erase(
            std::remove_if(collisionEffects.begin(), collisionEffects.end(),
                           [](const CollisionEffect& c) { return c.isFinished(); }),
            collisionEffects.end());

    smokeCloud.update(dt);

    // Update brake trails
    for (auto& trail : brakeTrails) trail.update(dt);
    brakeTrails.erase(
            std::remove_if(brakeTrails.begin(), brakeTrails.end(),
                            [](const BrakeTrail& t) { return !t.is_alive(); }),
            brakeTrails.end());

    // Add smoke is accelerating
    if (isAccelerating) {
        const auto& cars = world.getCars();
        auto it = cars.find(playerId);
        if (it != cars.end()) 
            smokeCloud.addAccelerationSmoke(it->second.x, it->second.y, it->second.angle);
    }

    // Extend trail if breaking
    if (isBraking && currentBrakeTrail) {
        const auto& cars = world.getCars();
        auto it = cars.find(playerId);
        if (it != cars.end()) 
            currentBrakeTrail->extend(it->second.x, it->second.y);
    }

}


void WorldRenderer::render() {
    renderMapCamera();

    renderBrakeTrails();
    renderCheckpoints();
    renderCarsUnderBridge();
    renderSmoke(); 

    renderBridges();
    renderCarsOnBridge();


    renderCollisionEffects();
    renderExplosions();
}


void WorldRenderer::renderMapCamera() {
    renderer.SetScale(scaleFactor, scaleFactor);
    renderer.Copy(mapTexture, camera, NullOpt);
}

void WorldRenderer::renderCarsUnderBridge() {
    const auto& cars = world.getCars();
    for (const auto& [id, carState] : cars) {
        if (!carState.onBridge) renderCar(carState);
    }
}


void WorldRenderer::renderBridges() {
    if (bridgeRenderer)
        bridgeRenderer->render(camera, scaleFactor);
}


void WorldRenderer::renderCarsOnBridge() {
    const auto& cars = world.getCars();
    for (const auto& [id, carState] : cars) {
        if (carState.onBridge) renderCar(carState);
    }
}

void WorldRenderer::renderCar(const BroadcastData::CarState& carState) {
    const Rect& src = CARS[carState.type];

    float screenX = carState.x - camera.x - src.GetW() / 2.0f;
    float screenY = carState.y - camera.y - src.GetH() / 2.0f;

    Rect dest(screenX, screenY, src.GetW(), src.GetH());
    SDL_Point center = {src.GetW() / 2, src.GetH() / 2};
    
    renderer.Copy(carSprites, src, dest, carState.angle, center, SDL_FLIP_NONE);
}


void WorldRenderer::renderCheckpoints() {
    const auto& cars = world.getCars();
    auto it = cars.find(playerId);
    if (it != cars.end())
        checkpointRenderer.render(it->second.checkpoints, camera);
}

void WorldRenderer::renderExplosions() {
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);

    for (const auto& explosion: explosions) {
        for (const auto& particle: explosion.particles) {
            if (!particle.isAlive())
                continue;

            float screenX = particle.x - camera.x;
            float screenY = particle.y - camera.y;
            
            // Color with alpha based on remaining life
            SDL_Color color = particle.color;
            color.a = static_cast<Uint8>(particle.life * 255);

            renderer.SetDrawColor(color.r, color.g, color.b, color.a);

            // Particle as a small circle
            int size = static_cast<int>(particle.size * particle.life);
            if (size < 1)
                size = 1;

            SDL_Rect rect = {static_cast<int>(screenX - size / 2),
                             static_cast<int>(screenY - size / 2), size, size};

            renderer.FillRect(rect);
        }
    }

    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
}

void WorldRenderer::renderCollisionEffects() {
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);

    for (const auto& collision: collisionEffects) {
        float screenX = collision.x - camera.x;
        float screenY = collision.y - camera.y;

        // White flash that fades out
        float alpha = (1.0f - collision.timeAlive / 0.3f) * collision.intensity;
        Uint8 alphaByte = static_cast<Uint8>(alpha * 255);

        renderer.SetDrawColor(255, 255, 255, alphaByte);
        int radius = static_cast<int>(collision.timeAlive * 100 * collision.intensity);

        // Draw simple circle (lines)
        for (int angle = 0; angle < 360; angle += 10) {
            float rad = angle * 3.14159f / 180.0f;
            int x1 = screenX + cos(rad) * radius;
            int y1 = screenY + sin(rad) * radius;
            int x2 = screenX + cos((angle + 10) * 3.14159f / 180.0f) * radius;
            int y2 = screenY + sin((angle + 10) * 3.14159f / 180.0f) * radius;

            renderer.DrawLine(x1, y1, x2, y2);
        }
    }

    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
}

void WorldRenderer::addExplosion(float x, float y, int particleCount) {
    explosions.emplace_back(x, y, particleCount);
}

void WorldRenderer::addCollisionEffect(float x, float y, float intensity) {
    collisionEffects.emplace_back(x, y, intensity);
}

void WorldRenderer::setAccelerating(bool accelerating) {
    isAccelerating = accelerating;
}

void WorldRenderer::setBraking(bool braking) {
    // new trail when the car starts breaking
    if (braking && !isBraking) {
        const auto& cars = world.getCars();
        auto it = cars.find(playerId);
        if (it != cars.end()) {
            brakeTrails.emplace_back(it->second.x, it->second.y);
            currentBrakeTrail = &brakeTrails.back();
        }
    }

    if (!braking && isBraking) 
        currentBrakeTrail = nullptr;

    isBraking = braking;
}

void WorldRenderer::addExplosionSmoke(float x, float y) {
    smokeCloud.addExplosionSmoke(x, y);
}

void WorldRenderer::renderSmoke() {
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    
    for (const auto& particle : smokeCloud.particles) {
        if (!particle.is_alive())
            continue;
        
        float screenX = particle.x - camera.x;
        float screenY = particle.y - camera.y;
        renderer.SetDrawColor(100, 100, 100, particle.alpha);
        
        int size = static_cast<int>(particle.size);
        SDL_Rect rect = {
            static_cast<int>(screenX - size / 2),
            static_cast<int>(screenY - size / 2),
            size, size
        };
        renderer.FillRect(rect);
    }
    
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
}

void WorldRenderer::renderBrakeTrails() {
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(30, 30, 30, 255);
    
    for (const auto& trail : brakeTrails) {
        if (!trail.is_alive() || trail.points.size() < 2)
            continue;

        renderer.SetDrawColor(30, 30, 30, trail.alpha);
        
        for (size_t i = 0; i < trail.points.size() - 1; i++) {
            float x1 = trail.points[i].first - camera.x;
            float y1 = trail.points[i].second - camera.y;
            float x2 = trail.points[i + 1].first - camera.x;
            float y2 = trail.points[i + 1].second - camera.y;
            
            float dx = x2 - x1;
            float dy = y2 - y1;
            float len = std::sqrt(dx*dx + dy*dy);
            
            if (len > 0.1f) {
                float perpX = -dy / len;
                float perpY = dx / len;
                
                // Wheels width
                float offset = 6.0f;  // 6 px
                
                // Left line
                int lx1 = static_cast<int>(x1 + perpX * offset);
                int ly1 = static_cast<int>(y1 + perpY * offset);
                int lx2 = static_cast<int>(x2 + perpX * offset);
                int ly2 = static_cast<int>(y2 + perpY * offset);
                
                // width 3px
                for (int w = -1; w <= 1; w++)
                    renderer.DrawLine(lx1 + w, ly1, lx2 + w, ly2);
                
                // Right line
                int rx1 = static_cast<int>(x1 - perpX * offset);
                int ry1 = static_cast<int>(y1 - perpY * offset);
                int rx2 = static_cast<int>(x2 - perpX * offset);
                int ry2 = static_cast<int>(y2 - perpY * offset);
                
                for (int w = -1; w <= 1; w++)
                    renderer.DrawLine(rx1 + w, ry1, rx2 + w, ry2);
            }
        }
    }
    
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
}