#include "bridgeRenderer.h"


BridgeRenderer::BridgeRenderer(Renderer& renderer, Texture& bridgeTexture)
    : renderer(renderer), bridgeTexture(bridgeTexture) {}

void BridgeRenderer::render(const Rect& camera, float scale) {
    renderer.SetScale(scale, scale);
    
    // Render the bridge texture with the same camera offset as the map
    renderer.Copy(bridgeTexture, camera, NullOpt);
}