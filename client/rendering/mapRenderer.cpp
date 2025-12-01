#include "mapRenderer.h"


MapRenderer::MapRenderer(Renderer& renderer, Texture& mapTexture)
    : renderer(renderer), mapTexture(mapTexture) {}


void MapRenderer::render(const Rect& camera) {
    renderer.Copy(mapTexture, camera, NullOpt);
}
