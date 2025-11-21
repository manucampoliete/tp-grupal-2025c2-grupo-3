#include "worldRenderer.h"


WorldRenderer::WorldRenderer(Renderer& renderer, Texture& mapTexture, Texture& carSprites,
                             World& world, uint8_t playerId):
        renderer(renderer),
        mapTexture(mapTexture),
        carSprites(carSprites),
        world(world),
        playerId(playerId),  // Manu's note: not used
        camera(0, 0, 800, 600),
        scaleFactor(1.0f) {}


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
    if (camera.x < 0)
        camera.x = 0;
    if (camera.y < 0)
        camera.y = 0;
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
}


void WorldRenderer::render() {
    renderMapCamera();
    renderAllCars();
    renderCollisionEffects();
    renderExplosions();
}


void WorldRenderer::renderMapCamera() {
    renderer.SetScale(scaleFactor, scaleFactor);
    renderer.Copy(mapTexture, camera, NullOpt);
}


void WorldRenderer::renderAllCars() {
    auto cars = world.getCars();
    for (const auto& [id, car_state]: cars) {
        const Rect& src = CARS[car_state.type];

        float screenX = car_state.x - camera.x - src.GetW() / 2.0f;
        float screenY = car_state.y - camera.y - src.GetH() / 2.0f;

        std::cout << "  Car id=" << (int)id << ", type=" << (int)car_state.type << ", worldPos=("
                  << car_state.x << "," << car_state.y << ")"
                  << ", screenPos=(" << screenX << "," << screenY << ")"
                  << ", angle=" << car_state.angle << std::endl;

        Rect dest(screenX, screenY, src.GetW(), src.GetH());
        SDL_Point center = {src.GetW() / 2, src.GetH() / 2};
        renderer.Copy(carSprites, src, dest, car_state.angle, center, SDL_FLIP_NONE);
    }
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
