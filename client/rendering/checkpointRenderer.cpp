#include "checkpointRenderer.h"
#include <iostream>

#include "../../common/utils/pathElements.h"


CheckpointRenderer::CheckpointRenderer(Renderer& renderer, Font& fontBig) :
    renderer(renderer),
    arrowTexture(renderer, "client/assets/checkpoints/arrow_up.png"),
    curveTexture(renderer, "client/assets/checkpoints/curve_down_right.png"),
    checkpointTexture(renderer, "client/assets/checkpoints/cp_horizontal.png"),
    finishTexture(renderer, "client/assets/checkpoints/finish_horizontal.png"),
    startTexture(renderer, "client/assets/checkpoints/start_horizontal.png"),
    finishImg(renderer, "client/assets/checkpoints/finish.png"), 
    fontBig(fontBig) {

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
                angle = 180.0;
                flip = SDL_FLIP_HORIZONTAL;
                break;
            case DIR_CURVE_UP_LEFT:
                textureToUse = &curveTexture;
                angle = 180.0;
                flip = SDL_FLIP_NONE;
                break;
            case DIR_CURVE_DOWN_RIGHT:
                textureToUse = &curveTexture;
                angle = 0.0;
                flip = SDL_FLIP_NONE;
                break;
            case DIR_CURVE_DOWN_LEFT:
                textureToUse = &curveTexture;
                angle = 0.0;
                flip = SDL_FLIP_HORIZONTAL;
                break;
            case DIR_CURVE_RIGHT_UP:
                textureToUse = &curveTexture;
                angle = 270.0;
                flip = SDL_FLIP_NONE;
                break;
            case DIR_CURVE_LEFT_UP:
                textureToUse = &curveTexture;
                angle = 90.0;
                flip = SDL_FLIP_HORIZONTAL;
                break;
            case DIR_CURVE_RIGHT_DOWN:
                textureToUse = &curveTexture;
                angle = 270.0;
                flip = SDL_FLIP_HORIZONTAL;
                break;
            case DIR_CURVE_LEFT_DOWN:
                textureToUse = &curveTexture;
                angle = 90.0;
                flip = SDL_FLIP_NONE;
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
            int w = 50;
            int h = 50;

            Rect dest(screenX - w / 2, screenY - h / 2, w, h);

            renderer.Copy(*textureToUse, NullOpt, dest, angle, NullOpt, flip);
        }
    }
}

void CheckpointRenderer::renderFinishedPopup() {
    renderer.SetScale(1.0f, 1.0f);

    int w = renderer.GetOutputWidth();
    int h = renderer.GetOutputHeight();

    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 180);
    renderer.FillRect(Rect(0, 0, w, h));

    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);

    std::string title = "YOU FINISHED THE RACE!";
    SDL_Color titleColor = {0, 200, 0, 255};
    Texture* img = &finishImg;

    // Render PNG with transparency
    if (img) {
        int imgW = std::min(img->GetWidth(), w / 2);
        int imgH = (imgW * img->GetHeight()) / img->GetWidth();

        int imgX = (w - imgW) / 2;
        int imgY = (h - imgH) / 2 + 20;

        renderer.Copy(*img, NullOpt, Rect(imgX, imgY, imgW, imgH));
    }

    // Title below the image
    Surface titleSurface = fontBig.RenderText_Solid(title, titleColor);
    Texture titleTexture(renderer, titleSurface);

    int titleX = (w - titleTexture.GetWidth()) / 2;
    int titleY = 100;

    if (!img)
        titleY = (h - titleTexture.GetHeight()) / 2;

    renderer.Copy(titleTexture, NullOpt,
                  Rect(titleX, titleY, titleTexture.GetWidth(), titleTexture.GetHeight()));
}
