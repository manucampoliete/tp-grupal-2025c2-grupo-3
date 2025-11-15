#include "world_renderer.h"


WorldRenderer::WorldRenderer(Renderer& renderer, Texture& map_texture, Texture& car_sprites,
                             World& world, uint8_t player_id):
        renderer(renderer),
        map_texture(map_texture),
        car_sprites(car_sprites),
        world(world),
        player_id(player_id),
        camera(0, 0, 800, 600),
        scale_factor(1.0f) {}


void WorldRenderer::update_layout(int window_width, int window_height) {
    // actualizo el factor de escala
    scale_factor = (float)window_height / REFERENCE_HEIGHT;

    // actualizo la cámara para que coincida con el tamaño de la ventana
    // pero escalado inversamente (si la ventana es 2x, la cámara ve la mitad)
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


void WorldRenderer::update_effects(float dt) {
    // actualizar explosiones
    for (auto& explosion : explosions)
        explosion.update(dt);

    explosions.erase(
        std::remove_if(explosions.begin(), explosions.end(),
                      [](const Explosion& e) { return e.is_finished(); }),
        explosions.end()
    );
    
    // axctualizar colisiones
    for (auto& collision : collision_effects)
        collision.update(dt);

    collision_effects.erase(
        std::remove_if(collision_effects.begin(), collision_effects.end(),
                      [](const CollisionEffect& c) { return c.is_finished(); }),
        collision_effects.end()
    );
}


void WorldRenderer::render() {
    render_map_camera();
    render_all_cars();
    render_collision_effects();
    render_explosions();
}


void WorldRenderer::render_map_camera() {
    renderer.SetScale(scale_factor, scale_factor);
    renderer.Copy(map_texture, camera, NullOpt);
}


void WorldRenderer::render_all_cars() {
    auto cars = world.getCars();
    for (const auto& [id, car_state]: cars) {
        const Rect& src = CARS[car_state.type];

        float screen_x = car_state.x - camera.x - src.GetW() / 2.0f;
        float screen_y = car_state.y - camera.y - src.GetH() / 2.0f;
        
        std::cout << "  Auto id=" << (int)id  << ", type=" << (int)car_state.type << ", world_pos=(" << car_state.x << "," << car_state.y << ")" << ", screen_pos=(" << screen_x << "," << screen_y << ")" << ", angle=" << car_state.angle << std::endl;
        
        Rect dest(screen_x, screen_y, src.GetW(), src.GetH());
        SDL_Point center = {src.GetW() / 2, src.GetH() / 2};
        renderer.Copy(car_sprites, src, dest, car_state.angle, center, SDL_FLIP_NONE);
    }
}


void WorldRenderer::render_explosions() {
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    
    for (const auto& explosion : explosions) {
        for (const auto& particle : explosion.particles) {
            if (!particle.is_alive()) continue;
            
            float screen_x = particle.x - camera.x;
            float screen_y = particle.y - camera.y;
            
            // color con alpha basado en vida restante
            SDL_Color color = particle.color;
            color.a = static_cast<Uint8>(particle.life * 255);
            
            renderer.SetDrawColor(color.r, color.g, color.b, color.a);
            
            // partícula como un circulo chiquito
            int size = static_cast<int>(particle.size * particle.life);
            if (size < 1) size = 1;
            
            SDL_Rect rect = {static_cast<int>(screen_x - size/2), static_cast<int>(screen_y - size/2), size, size};
            
            renderer.FillRect(rect);
        }
    }
    
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
}

void WorldRenderer::render_collision_effects() {
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    
    for (const auto& collision : collision_effects) {
        float screen_x = collision.x - camera.x;
        float screen_y = collision.y - camera.y;
        
        // flash blanco que se desvanece
        float alpha = (1.0f - collision.time_alive / 0.3f) * collision.intensity;
        Uint8 alpha_byte = static_cast<Uint8>(alpha * 255);
        
        renderer.SetDrawColor(255, 255, 255, alpha_byte);
        int radius = static_cast<int>(collision.time_alive * 100 * collision.intensity);
        
        // dibujar círculo simple (líneas)
        for (int angle = 0; angle < 360; angle += 10) {
            float rad = angle * 3.14159f / 180.0f;
            int x1 = screen_x + cos(rad) * radius;
            int y1 = screen_y + sin(rad) * radius;
            int x2 = screen_x + cos((angle + 10) * 3.14159f / 180.0f) * radius;
            int y2 = screen_y + sin((angle + 10) * 3.14159f / 180.0f) * radius;
            
            renderer.DrawLine(x1, y1, x2, y2);
        }
    }
    
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
}

void WorldRenderer::add_explosion(float x, float y, int particle_count) {
    explosions.emplace_back(x, y, particle_count);
}

void WorldRenderer::add_collision_effect(float x, float y, float intensity) {
    collision_effects.emplace_back(x, y, intensity);
}