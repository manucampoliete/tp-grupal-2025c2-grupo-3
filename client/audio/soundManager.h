#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <chrono>
#include <map>
#include <string>
#include <memory>

#include <SDL2pp/SDL2pp.hh>
#include <SDL2pp/Mixer.hh>
#include <SDL2pp/Music.hh>
#include <SDL2pp/Chunk.hh>


class SoundManager {
private:
    SDL2pp::Mixer mixer;
    
    std::unique_ptr<SDL2pp::Music> backgroundMusic;
    std::map<std::string, std::unique_ptr<SDL2pp::Chunk>> soundEffects;

    bool musicEnabled = true;
    bool sfxEnabled = true;
    int musicVolume = MIX_MAX_VOLUME / 2;  // 64
    int sfxVolume = MIX_MAX_VOLUME;        // 128

    // Throttle to avoid sound saturation
    std::map<std::string, std::chrono::steady_clock::time_point> lastPlayTime;
    const int THROTTLE_MS = 100;

    int engineChannel = -1;
    bool enginePlaying = false;

    int brakeChannel = -1;
    bool brakePlaying = false;

    bool canPlaySound(const std::string& name);

public:
    SoundManager();
    ~SoundManager() = default;

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
    void stopBraking();

    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

};

#endif  // SOUND_MANAGER_H
