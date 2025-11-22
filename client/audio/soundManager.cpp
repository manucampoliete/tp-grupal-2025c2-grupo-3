#include "soundManager.h"

#include <algorithm>
#include <iostream>


SoundManager::SoundManager(): backgroundMusic(nullptr) {
    // Initialize SDL_mixer
    if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 4096) < 0) 
        throw std::runtime_error("Failed to initialize SDL_mixer");

    // 16 channels for sound effects
    Mix_AllocateChannels(16);
}

SoundManager::~SoundManager() {
    stopMusic();

    if (backgroundMusic) {
        Mix_FreeMusic(backgroundMusic);
        backgroundMusic = nullptr;
    }

    for (auto& [name, chunk]: soundEffects) Mix_FreeChunk(chunk);

    soundEffects.clear();

    Mix_CloseAudio();
    std::cout << "[SOUND] Mixer closed" << std::endl;
}


// BACKGROUND MUSIC FOR RACING
void SoundManager::loadMusic(const std::string& path) {
    if (backgroundMusic)
        Mix_FreeMusic(backgroundMusic);

    backgroundMusic = Mix_LoadMUS(path.c_str());
}

void SoundManager::playMusic(int loops) {
    if (backgroundMusic && musicEnabled) {
        if (Mix_PlayMusic(backgroundMusic, loops) == -1)
            std::cerr << "[SOUND] Error playing music: " << Mix_GetError() << std::endl;
        else
            Mix_VolumeMusic(musicVolume);
    }
}

void SoundManager::stopMusic() { Mix_HaltMusic(); }

void SoundManager::pauseMusic() {
    if (Mix_PlayingMusic())
        Mix_PauseMusic();
}

void SoundManager::resumeMusic() {
    if (Mix_PausedMusic())
        Mix_ResumeMusic();
}

void SoundManager::setMusicVolume(int volume) {
    musicVolume = std::clamp(volume, 0, MIX_MAX_VOLUME);
    Mix_VolumeMusic(musicVolume);
}


// SOUND EFFECTS
void SoundManager::loadSound(const std::string& name, const std::string& path) {
    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    if (!chunk) 
        return;

    soundEffects[name] = chunk;
}

void SoundManager::playSound(const std::string& name, int volume) {
    if (!sfxEnabled)
        return;

    // Throttling: avoid spamming the same sound
    if (!canPlaySound(name))
        return;

    auto it = soundEffects.find(name);
    if (it != soundEffects.end()) {
        int vol = (volume == -1) ? sfxVolume : std::clamp(volume, 0, MIX_MAX_VOLUME);
        Mix_VolumeChunk(it->second, vol);

        int channel = Mix_PlayChannel(-1, it->second, 0);  // -1 = first free channel
        if (channel == -1)
            std::cerr << "[SOUND] No free channels to play: " << name << std::endl;
    }
}

void SoundManager::playSoundWithDistance(const std::string& name, float distance,
                                            float maxDistance) {
    if (!sfxEnabled)
        return;

    // Calculate volume based on distance
    // distance = 0 → max volume
    // distance >= maxDistance → vol 0
    float volumeFactor = 1.0f - std::min(distance / maxDistance, 1.0f);
    int volume = static_cast<int>(volumeFactor * sfxVolume);

    if (volume > 5)  // Play only if volume is significant
        playSound(name, volume);
}

void SoundManager::setSfxVolume(int volume) {
    sfxVolume = std::clamp(volume, 0, MIX_MAX_VOLUME);
}


// CONTROL
void SoundManager::toggleMusic() {
    musicEnabled = !musicEnabled;
    if (!musicEnabled) 
        pauseMusic();
    else 
        resumeMusic();
}

void SoundManager::toggleSfx() {
    sfxEnabled = !sfxEnabled;
}


// EXTRA
bool SoundManager::canPlaySound(const std::string& name) {
    auto now = std::chrono::steady_clock::now();
    auto it = lastPlayTime.find(name);
    if (it == lastPlayTime.end()) {
        lastPlayTime[name] = now;
        return true;
    }

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second).count();
    if (elapsed >= THROTTLE_MS) {
        lastPlayTime[name] = now;
        return true;
    }

    return false;
}


void SoundManager::playEngineLoop() {
    if (!sfxEnabled || enginePlaying)
        return;
    
    auto it = soundEffects.find("engine");
    if (it != soundEffects.end()) {
        // Loop infinito en un canal específico
        engineChannel = Mix_PlayChannel(-1, it->second, -1);  // -1 = loop infinito
        if (engineChannel != -1) {
            Mix_Volume(engineChannel, sfxVolume);
            enginePlaying = true;
        }
    }
}

void SoundManager::stopEngineLoop() {
    if (engineChannel != -1 && enginePlaying) {
        Mix_HaltChannel(engineChannel);
        enginePlaying = false;
        engineChannel = -1;
    }
}

void SoundManager::playBrakeSound() {  
    playSound("brake", sfxVolume * 2);
}
