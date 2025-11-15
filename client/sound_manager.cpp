#include "sound_manager.h"

#include <algorithm>
#include <iostream>

SoundManager::SoundManager(): background_music(nullptr) {
    // inicializar SDL_mixer
    if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        std::cerr << "[SOUND] Error inicializando SDL_mixer: " << Mix_GetError() << std::endl;
        throw std::runtime_error("Failed to initialize SDL_mixer");
    }

    // 16 canales para efectos de sonido
    Mix_AllocateChannels(16);
    std::cout << "[SOUND] mixer inicializado correctamente" << std::endl;
}

SoundManager::~SoundManager() {
    stop_music();

    if (background_music) {
        Mix_FreeMusic(background_music);
        background_music = nullptr;
    }

    for (auto& [name, chunk]: sound_effects) Mix_FreeChunk(chunk);

    sound_effects.clear();

    Mix_CloseAudio();
    std::cout << "[SOUND] mixer cerrado" << std::endl;
}


// MUSICA DE FONDO DE CARRERA

void SoundManager::load_music(const std::string& path) {
    if (background_music)
        Mix_FreeMusic(background_music);

    background_music = Mix_LoadMUS(path.c_str());
    if (!background_music)
        std::cerr << "[SOUND] Error cargando música: " << path << " - " << Mix_GetError()
                  << std::endl;
}

void SoundManager::play_music(int loops) {
    if (background_music && music_enabled) {
        if (Mix_PlayMusic(background_music, loops) == -1)
            std::cerr << "[SOUND] Error reproduciendo música: " << Mix_GetError() << std::endl;
        else
            Mix_VolumeMusic(music_volume);
    }
}

void SoundManager::stop_music() { Mix_HaltMusic(); }

void SoundManager::pause_music() {
    if (Mix_PlayingMusic())
        Mix_PauseMusic();
}

void SoundManager::resume_music() {
    if (Mix_PausedMusic())
        Mix_ResumeMusic();
}

void SoundManager::set_music_volume(int volume) {
    music_volume = std::clamp(volume, 0, MIX_MAX_VOLUME);
    Mix_VolumeMusic(music_volume);
}


// EFECTOS DE SONIDO

void SoundManager::load_sound(const std::string& name, const std::string& path) {
    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    if (!chunk) {
        std::cerr << "[SOUND] Error cargando sonido '" << name << "': " << path << " - "
                  << Mix_GetError() << std::endl;
        return;
    }
    sound_effects[name] = chunk;
    std::cout << "[SOUND] Sonido cargado: " << name << " (" << path << ")" << std::endl;
}

void SoundManager::play_sound(const std::string& name, int volume) {
    if (!sfx_enabled)
        return;

    // throttling: evitar spam del mismo sonido
    if (!can_play_sound(name))
        return;

    auto it = sound_effects.find(name);
    if (it != sound_effects.end()) {
        int vol = (volume == -1) ? sfx_volume : std::clamp(volume, 0, MIX_MAX_VOLUME);
        Mix_VolumeChunk(it->second, vol);

        int channel = Mix_PlayChannel(-1, it->second, 0);  // -1 = primer canal libre
        if (channel == -1)
            std::cerr << "[SOUND] No hay canales libres para reproducir: " << name << std::endl;
    } else {
        std::cerr << "[SOUND] Sonido no encontrado: " << name << std::endl;
    }
}

void SoundManager::play_sound_with_distance(const std::string& name, float distance,
                                            float max_distance) {
    if (!sfx_enabled)
        return;

    // calcular vol basado en la distancia
    // distance = 0 → vol máximo
    // distance >= max_distance → vol 0
    float volume_factor = 1.0f - std::min(distance / max_distance, 1.0f);
    int volume = static_cast<int>(volume_factor * sfx_volume);

    if (volume > 5)  // reproducir solo si el vol es significativo
        play_sound(name, volume);
}

void SoundManager::set_sfx_volume(int volume) {
    sfx_volume = std::clamp(volume, 0, MIX_MAX_VOLUME);
}


// CONTROL

void SoundManager::toggle_music() {
    music_enabled = !music_enabled;
    if (!music_enabled) {
        pause_music();
        std::cout << "[SOUND] Música desactivada" << std::endl;
    } else {
        resume_music();
        std::cout << "[SOUND] Música activada" << std::endl;
    }
}

void SoundManager::toggle_sfx() {
    sfx_enabled = !sfx_enabled;
    std::cout << "[SOUND] " << (sfx_enabled ? "🔊" : "🔇") << " Efectos de sonido "
              << (sfx_enabled ? "activados" : "desactivados") << std::endl;
}


// EXTRA

bool SoundManager::can_play_sound(const std::string& name) {
    auto now = std::chrono::steady_clock::now();
    auto it = last_play_time.find(name);
    if (it == last_play_time.end()) {
        last_play_time[name] = now;
        return true;
    }

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second).count();
    if (elapsed >= THROTTLE_MS) {
        last_play_time[name] = now;
        return true;
    }

    return false;
}
