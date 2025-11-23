#include "effectsManager.h"


void EffectsManager::triggerScreenFlash() {
    screenFlashActive = true;
    flashTimer = FLASH_DURATION;
}

void EffectsManager::update(float dtSeconds) {
    if (screenFlashActive) {
        flashTimer -= dtSeconds;
        if (flashTimer <= 0) {
            flashTimer = 0;
            screenFlashActive = false;
        }
    }
}

void EffectsManager::render(Renderer& renderer, int windowWidth, int windowHeight) {
    if (!screenFlashActive) return;

    renderer.SetScale(1.0f, 1.0f);
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);

    // alpha fades out as timer decreases
    float alpha = (flashTimer / FLASH_DURATION) * 200.0f;
    renderer.SetDrawColor(255, 255, 255, static_cast<Uint8>(alpha));
    renderer.FillRect(Rect(0, 0, windowWidth, windowHeight));

    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
}