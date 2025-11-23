#ifndef EFFECTS_MANAGER_H
#define EFFECTS_MANAGER_H

#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Window.hh>

#include "../utils/gameData.h"

using namespace SDL2pp;

/**
 * EffectsManager: nandles screen-level visual effects
 * - Screen flash (on high-impact collisions)
 
 */
class EffectsManager {
private:
    bool screenFlashActive = false;
    float flashTimer = 0.0f;
    static constexpr float FLASH_DURATION = 0.2f;

public:
    EffectsManager() = default;

    void triggerScreenFlash();
    
    void update(float dtSeconds);
    
    void render(Renderer& renderer, int windowWidth, int windowHeight);
    
    bool isFlashActive() const { return screenFlashActive; }
};

#endif  // EFFECTS_MANAGER_H