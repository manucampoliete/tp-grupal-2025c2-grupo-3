#include "soundManager.h"

#include <algorithm>
#include <iostream>

SoundManager::SoundManager(): 
    mixer(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096),
    backgroundMusic(nullptr),
    engineChannel(-1),
    enginePlaying(false) {

    // 16 channels for sound effects
    mixer.AllocateChannels(16);
}


// BACKGROUND MUSIC FOR RACING
void SoundManager::loadMusic(const std::string& path) {
    try {
        backgroundMusic = std::make_unique<SDL2pp::Music>(path);
    } catch (const SDL2pp::Exception& e) {
        std::cerr << "[SOUND] Error loading music: " << path << " - " << e.what() << std::endl;
        backgroundMusic = nullptr;
    }
}

void SoundManager::playMusic(int loops) {
    if (backgroundMusic && musicEnabled) {
        try {
            mixer.SetMusicVolume(musicVolume);
            mixer.PlayMusic(*backgroundMusic, loops);
        } catch (const SDL2pp::Exception& e) {
            std::cerr << "[SOUND] Error playing music: " << e.what() << std::endl;
        }
    }
}

void SoundManager::stopMusic() { mixer.HaltMusic(); }

void SoundManager::pauseMusic() {
    if (Mix_PlayingMusic())
        mixer.PauseMusic();
}

void SoundManager::resumeMusic() {
    if (Mix_PausedMusic())
        mixer.ResumeMusic();
}

void SoundManager::setMusicVolume(int volume) {
    musicVolume = std::clamp(volume, 0, MIX_MAX_VOLUME);
    mixer.SetMusicVolume(musicVolume);
}


// SOUND EFFECTS
void SoundManager::loadSound(const std::string& name, const std::string& path) {
    try {
        soundEffects[name] = std::make_unique<SDL2pp::Chunk>(path);
    } catch (const SDL2pp::Exception& e) {
        std::cerr << "[SOUND] Error loading sound '" << name << "': " << e.what() << std::endl;
    }
}

void SoundManager::playSound(const std::string& name, int volume) {
    if (!sfxEnabled)
        return;

    // Throttling: avoid spamming the same sound
    if (!canPlaySound(name))
        return;

    auto it = soundEffects.find(name);
    if (it != soundEffects.end() && it->second) {
        int vol = (volume == -1) ? sfxVolume : std::clamp(volume, 0, MIX_MAX_VOLUME);
        it->second->SetVolume(vol);

        try {
            mixer.PlayChannel(-1, *it->second);  // -1 = primer canal libre
        } catch (const SDL2pp::Exception& e) {
            std::cerr << "[SOUND] No free channels to play: " << name << std::endl;
        }
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
    if (!sfxEnabled && enginePlaying)
        stopEngineLoop();
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


// SPECIAL SOUND EFFECTS
void SoundManager::playEngineLoop() {
    if (!sfxEnabled || enginePlaying)
        return;
    
    auto it = soundEffects.find("engine");
    if (it != soundEffects.end() && it->second) {
        try {
            it->second->SetVolume(sfxVolume);
            engineChannel = mixer.PlayChannel(-1, *it->second, -1);  // -1 = loop infinito
            enginePlaying = true;
        } catch (const SDL2pp::Exception& e) {
            std::cerr << "[SOUND] Error playing engine loop: " << e.what() << std::endl;
        }
    }
}

void SoundManager::stopEngineLoop() {
    if (engineChannel != -1 && enginePlaying) {
        mixer.HaltChannel(engineChannel);
        enginePlaying = false;
        engineChannel = -1;
    }
}

void SoundManager::playBrakeSound() {  
    playSound("brake", sfxVolume * 2);
}
