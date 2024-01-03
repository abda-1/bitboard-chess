#ifndef AUDIOMANAGER_HPP
#define AUDIOMANAGER_HPP

#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>
#include <unordered_map>
#include <string>

enum AudioType {
    START, CAPTURE, MOVE, CHECK, CHECKMATE
};

class AudioManager {

    private:
        std::unordered_map<AudioType, Mix_Chunk*> soundEffects;
        bool loadSound(AudioType name, const std::string& path);

    public:
        AudioManager();
        ~AudioManager();

        void playSound(AudioType name);
};

AudioManager::AudioManager() {

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }

    // Load all sounds upon initialisation
    loadSound(AudioType::START, "src/assets/audio/start.wav");
    loadSound(AudioType::MOVE, "src/assets/audio/move.wav");
    loadSound(AudioType::CHECK, "src/assets/audio/check.wav");
    loadSound(AudioType::CHECKMATE, "src/assets/audio/checkmate.wav");
    loadSound(AudioType::CAPTURE, "src/assets/audio/capture.wav");

}

AudioManager::~AudioManager() {
    for (auto& it : soundEffects) {
        Mix_FreeChunk(it.second);
    }
    soundEffects.clear();
    Mix_CloseAudio();
}

bool AudioManager::loadSound(AudioType name, const std::string& path) {

    Mix_Chunk* soundEffect = Mix_LoadWAV(path.c_str());
    if (soundEffect == nullptr) {
        std::cerr << "Failed to load sound effect: " << name << "! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }

    soundEffects[name] = soundEffect;
    return true;

}

void AudioManager::playSound(AudioType name) {
    auto it = soundEffects.find(name);
    if (it != soundEffects.end()) {
        Mix_PlayChannel(-1, it->second, 0);
    }
}


#endif // AUDIOMANAGER_HPP