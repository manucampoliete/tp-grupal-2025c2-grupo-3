#ifndef CHECKPOINT_RENDERER_H
#define CHECKPOINT_RENDERER_H

#include <SDL.h>
#include <SDL2pp/SDL2pp.hh>
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

public:
    explicit CheckpointRenderer(Renderer& renderer);

    void render(const std::vector<BroadcastData::CarState::Checkpoint>& checkpoints, 
                const Rect& camera);
    
};

#endif // CHECKPOINT_RENDERER_H