#ifndef MAP_RENDERER_H
#define MAP_RENDERER_H

#include <SDL2pp/SDL2pp.hh>
#include <SDL2pp/Texture.hh>
#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Rect.hh>

using namespace SDL2pp;


class MapRenderer {
private:
    Renderer& renderer;
    Texture& mapTexture; 

public:
    MapRenderer(Renderer& renderer, Texture& mapTexture);

    void render(const Rect& camera);
};

#endif  // MAP_RENDERER_H
