#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <SDL2/SDL_mixer.h>
#include <map>
#include <string>
#include <chrono>


class SoundManager {
private:
    Mix_Music* background_music;
    std::map<std::string, Mix_Chunk*> sound_effects;
    
    bool music_enabled = true;
    bool sfx_enabled = true;
    int music_volume = MIX_MAX_VOLUME / 2; // 64
    int sfx_volume = MIX_MAX_VOLUME; // 128
    
    // throttling para evitar saturación de sonidos
    std::map<std::string, std::chrono::steady_clock::time_point> last_play_time;
    const int THROTTLE_MS = 100;

public:
    SoundManager();
    ~SoundManager();
    
    // musica de fondo
    void load_music(const std::string& path);
    void play_music(int loops = -1); // -1 = loop infinito
    void stop_music();
    void pause_music();
    void resume_music();
    void set_music_volume(int volume); // 0-128
    
    // efecots de sonido
    void load_sound(const std::string& name, const std::string& path);
    void play_sound(const std::string& name, int volume = -1); // -1 = usar volumen por defecto
    void play_sound_with_distance(const std::string& name, float distance, float max_distance = 1000.0f);
    void set_sfx_volume(int volume);
    
    // control
    void toggle_music();
    void toggle_sfx();
    bool is_music_enabled() const { return music_enabled; }
    bool is_sfx_enabled() const { return sfx_enabled; }
    
private:
    bool can_play_sound(const std::string& name);
};

#endif // SOUND_MANAGER_H