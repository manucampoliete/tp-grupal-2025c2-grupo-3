#include "world_renderer.h"

WorldRenderer::WorldRenderer(Renderer& renderer, Texture& map_texture, Texture& car_sprites,
                             World& world, /* Car& player_car,*/ uint8_t player_id):
        renderer(renderer),
        map_texture(map_texture),
        car_sprites(car_sprites),
        world(world),
        /*player_car(player_car),*/
        player_id(player_id),
        camera(0, 0, 800, 600),
        scale_factor(1.0f) {}


void WorldRenderer::update_layout(int window_width, int window_height) {
    // actualizo el factor de escala
    scale_factor = (float)window_height / REFERENCE_HEIGHT;

    // actualizo la cámara para que coincida con el tamaño de la ventana
    // pero escalado inversamente (si la ventana es 2x, la cámara ve la mitad)
    // a chequear el zoom!!!
    camera.w = static_cast<int>(window_width / scale_factor);
    camera.h = static_cast<int>(window_height / scale_factor);
}


void WorldRenderer::update_camera(float player_x, float player_y) {
    camera.x = static_cast<int>(player_x - (camera.w / 2));
    camera.y = static_cast<int>(player_y - (camera.h / 2));

    // evito que la cámara se salga del mapa
    if (camera.x < 0)
        camera.x = 0;
    if (camera.y < 0)
        camera.y = 0;
    if (camera.x > map_texture.GetWidth() - camera.w)
        camera.x = map_texture.GetWidth() - camera.w;
    if (camera.y > map_texture.GetHeight() - camera.h)
        camera.y = map_texture.GetHeight() - camera.h;
}


void WorldRenderer::render() {
    render_map_camera();
    render_all_cars();
}


void WorldRenderer::render_map_camera() {
    // dibujp solo la porción de la cámara, escalada a toda la ventana
    // aplico el escalado manualmente al renderer asi todo lo que dibujo es a escala
    renderer.SetScale(scale_factor, scale_factor);

    // dibujo la porción del mapa que ve la camara
    renderer.Copy(map_texture, camera, NullOpt);
}


void WorldRenderer::render_all_cars() {
    auto cars = world.getCars();

    std::cout << "[WORLD_RENDERER] Renderizando " << cars.size()
              << " autos, player_id=" << (int)player_id << std::endl;

    for (const auto& [id, car_state]: cars) {
        const Rect& src = CARS[car_state.type];

        // posición relativa a la cámara
        float screen_x = car_state.x - camera.x;
        float screen_y = car_state.y - camera.y;

        std::cout << "  Auto id=" << (int)id << ", type=" << (int)car_state.type << ", world_pos=("
                  << car_state.x << "," << car_state.y << ")"
                  << ", screen_pos=(" << screen_x << "," << screen_y << ")"
                  << ", angle=" << car_state.angle << std::endl;

        // el tamaño es el del sprite original, el SetScale() del renderer lo agranda
        Rect dest(screen_x, screen_y, src.GetW(), src.GetH());

        /*
        if (id == player_id) {
            // el set_state de player_car ya tiene las coordenadas del mundo
            // el render() debe hacer la conversión a pantalla
            player_car.render(camera, 1.0f); // 1.0f porque el renderer ya escala
        } else {
            // renderiza otros autos
            const Rect& src = CARS[id];
            float screen_x = car_state.x - camera.x;
            float screen_y = car_state.y - camera.y;
            Rect dest(screen_x, screen_y, src.GetW(), src.GetH());

            renderer.Copy(car_sprites, src, dest, car_state.angle);
        }
        */

        SDL_Point center = {src.GetW() / 2, src.GetH() / 2};

        // Renderizar el auto (TODOS se renderizan igual)
        renderer.Copy(car_sprites, src, dest, car_state.angle, center, SDL_FLIP_NONE);
    }
}
