#include "checkpointRenderer.h"
#include <iostream>


CheckpointRenderer::CheckpointRenderer(Renderer& renderer) :
    renderer(renderer),
    arrowTexture(renderer, "client/assets/checkpoints/arrow_up.png"),
    curveTexture(renderer, "client/assets/checkpoints/curve_down_right.png"),
    checkpointTexture(renderer, "client/assets/checkpoints/cp_horizontal.png"),
    finishTexture(renderer, "client/assets/checkpoints/finish_horizontal.png"),
    startTexture(renderer, "client/assets/checkpoints/start_horizontal.png") {

    arrowTexture.SetBlendMode(SDL_BLENDMODE_BLEND);
    checkpointTexture.SetBlendMode(SDL_BLENDMODE_BLEND);
}


void CheckpointRenderer::render(
    const std::vector<BroadcastData::CarState::Checkpoint>& checkpoints, 
    const Rect& camera) {

    for (const auto& cp : checkpoints) {
        int screenX = static_cast<int>(cp.x - camera.x);
        int screenY = static_cast<int>(cp.y - camera.y);
        if (screenX < -200 || screenX > camera.w + 200 || 
            screenY < -200 || screenY > camera.h + 200) {
            continue;
        }

        double angle = 0.0;
        SDL_RendererFlip flip = SDL_FLIP_NONE;
        Texture* textureToUse = nullptr;

        // chequear que se impriman bien!!!

        switch (cp.id) {
            // HINTS (STRAIGHT ARROWS)
            case DIR_UP:
                textureToUse = &arrowTexture;
                angle = 0.0;
                break;
            case DIR_LEFT:
                textureToUse = &arrowTexture;
                angle = 270.0;
                break;
            case DIR_RIGHT:
                textureToUse = &arrowTexture;
                angle = 90.0;
                break;
            case DIR_DOWN:
                textureToUse = &arrowTexture;
                angle = 180.0;
                break;

            // HINTS (CURVE ARROWS)
            case DIR_CURVE_UP_RIGHT:
                textureToUse = &curveTexture;
                angle = 0.0;
                flip = SDL_FLIP_NONE;
                break;
            case DIR_CURVE_UP_LEFT:
                textureToUse = &curveTexture;
                angle = 0.0;
                flip = SDL_FLIP_HORIZONTAL;
                break;
            case DIR_CURVE_DOWN_RIGHT:
                textureToUse = &curveTexture;
                angle = 180.0;
                flip = SDL_FLIP_NONE;
                break;
            case DIR_CURVE_DOWN_LEFT:
                textureToUse = &curveTexture;
                angle = 180.0;
                flip = SDL_FLIP_HORIZONTAL;
                break;
            case DIR_CURVE_RIGHT_UP:
                textureToUse = &curveTexture;
                angle = 90.0;
                flip = SDL_FLIP_NONE;
                break;
            case DIR_CURVE_RIGHT_DOWN:
                textureToUse = &curveTexture;
                angle = 270.0;
                flip = SDL_FLIP_NONE;
                break;
            case DIR_CURVE_LEFT_UP:
                textureToUse = &curveTexture;
                angle = 90.0;
                flip = SDL_FLIP_HORIZONTAL;
                break;
            case DIR_CURVE_LEFT_DOWN:
                textureToUse = &curveTexture;
                angle = 270.0;
                flip = SDL_FLIP_HORIZONTAL;
                break;

            // START
            case START_HORIZONTAL:
                textureToUse = &startTexture;
                angle = 0.0;
                break;
            case START_VERTICAL:
                textureToUse = &startTexture;
                angle = 90.0;
                break;

            // CHECKPOINT
            case CHECKPOINT_HORIZONTAL:
                textureToUse = &checkpointTexture;
                angle = 0.0;
                break;
            case CHECKPOINT_VERTICAL:
                textureToUse = &checkpointTexture;
                angle = 90.0;
                break;

            // FINISH
            case FINISH_HORIZONTAL:
                textureToUse = &finishTexture;
                angle = 0.0;
                break;
            case FINISH_VERTICAL:
                textureToUse = &finishTexture;
                angle = 90.0;
                break;

            default:
                continue;
        }

        if (textureToUse) {
            int w = textureToUse->GetWidth();
            int h = textureToUse->GetHeight();

            Rect dest(screenX - w / 2, screenY - h / 2, w, h);

            renderer.Copy(*textureToUse, NullOpt, dest, angle, NullOpt, flip);
        }
    }
}