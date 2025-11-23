#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <SDL2pp/SDL2pp.hh>

#include "../../common/utils/gameState.h"

class GameLoop;
class GameStateManager;
class SoundManager;
class WorldRenderer;
class UIRenderer;

/**
 * InputHandler: processes all SDL input events
 * - Handle movement keys
 * - Handle key combinations (cheats and sound)
 * - Handle menu clicks (mod phase)
 * - Trigger appropriate effects (engine sound, brake trails)
 */
class InputHandler {
private:
    GameLoop& gameLoop;
    GameStateManager& stateManager;
    SoundManager& soundManager;
    WorldRenderer& worldRenderer;
    UIRenderer& uiRenderer;

    bool lastUp = false;
    bool lastDown = false;
    bool lastLeft = false;
    bool lastRight = false;

    void handleKeyDown(const SDL_Event& event, const Uint8* keyState);
    void handleMouseClick(const SDL_Event& event);
    void handleMovement(const Uint8* keyState);
    void handleCheats(SDL_Scancode key, const Uint8* keyState);
    void handleSoundControls(SDL_Scancode key, const Uint8* keyState);

public:
    InputHandler(GameLoop& gameLoop, GameStateManager& stateManager,
                 SoundManager& soundManager, WorldRenderer& worldRenderer,
                 UIRenderer& uiRenderer);

    bool processEvents(bool& windowResized);
};

#endif  // INPUT_HANDLER_H
