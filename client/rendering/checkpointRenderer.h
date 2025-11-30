#ifndef CHECKPOINT_RENDERER_H
#define CHECKPOINT_RENDERER_H

#include <SDL.h>
#include <SDL2pp/SDL2pp.hh>
#include <SDL2pp/Font.hh>
#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>
#include <SDL2pp/Rect.hh>
#include <vector>

#include "../utils/gameData.h"

using namespace SDL2pp;


class CheckpointRenderer {
private:
    Renderer& renderer;
    
    Texture arrowTexture;
    Texture curveTexture;
    Texture checkpointTexture;
    Texture finishTexture;
    Texture startTexture;
    Texture finishImg;

    Font& fontBig;

public:
    explicit CheckpointRenderer(Renderer& renderer, Font& fontBig);

    void render(const std::vector<BroadcastData::CarState::Checkpoint>& checkpoints, 
                const Rect& camera);
    
    void renderFinishedPopup();
};

#endif // CHECKPOINT_RENDERER_H