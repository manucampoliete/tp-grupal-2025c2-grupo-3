#ifndef BRIDGE_RENDERER_H
#define BRIDGE_RENDERER_H

#include <SDL2pp/SDL2pp.hh>
#include <SDL2pp/Texture.hh>
#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Rect.hh>

using namespace SDL2pp;


class BridgeRenderer {
private:
    Renderer& renderer;
    Texture& bridgeTexture; 

public:
    BridgeRenderer(Renderer& renderer, Texture& bridgeTexture);

    void render(const Rect& camera, float scale);
};

#endif  // BRIDGE_RENDERER_H