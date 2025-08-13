#pragma once
#include <fmod.hpp>
#include <unordered_map>
#include <string>

class FMODSoundManager
{
public:
    static FMODSoundManager& Instance();

    void Initialize();
    void Release();

    void LoadSound(const std::string& name, const std::string& path, bool loop = false);
    void PlaySound(const std::string& name, bool loop = false, float volume = 1.0f);
    void StopSound(const std::string& name);

    FMOD::System* system() { return system_; }


private:

    FMOD::System* system_ = nullptr;
    std::unordered_map<std::string, FMOD::Sound*> sounds_;
    std::unordered_map<std::string, FMOD::Channel*> channels_;
};
