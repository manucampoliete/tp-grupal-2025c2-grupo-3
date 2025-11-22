#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <chrono>
#include <map>
#include <string>

#include <SDL2/SDL_mixer.h>


class SoundManager {
private:
    Mix_Music* backgroundMusic;
    std::map<std::string, Mix_Chunk*> soundEffects;

    bool musicEnabled = true;
    bool sfxEnabled = true;
    int musicVolume = MIX_MAX_VOLUME / 2;  // 64
    int sfxVolume = MIX_MAX_VOLUME;        // 128

    // Throttle to avoid sound saturation
    std::map<std::string, std::chrono::steady_clock::time_point> lastPlayTime;
    const int THROTTLE_MS = 100;

    int engineChannel = -1;
    bool enginePlaying = false;

    bool canPlaySound(const std::string& name);

public:
    SoundManager();
    ~SoundManager();

    // Background music
    void loadMusic(const std::string& path);
    void playMusic(int loops = -1);  // -1 = infinite loop
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    void setMusicVolume(int volume);  // 0-128

    // Sound effects
    void loadSound(const std::string& name, const std::string& path);
    void playSound(const std::string& name, int volume = -1);  // -1 = use default volume
    void playSoundWithDistance(const std::string& name, float distance,
                                  float maxDistance = 1000.0f);
    void setSfxVolume(int volume);

    // Control
    void toggleMusic();
    void toggleSfx();
    bool isMusicEnabled() const { return musicEnabled; }
    bool isSfxEnabled() const { return sfxEnabled; }

    void playEngineLoop();
    void stopEngineLoop();
    void playBrakeSound();

};

#endif  // SOUND_MANAGER_H
