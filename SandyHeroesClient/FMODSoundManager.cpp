#include "FMODSoundManager.h"
#include <fmod_errors.h>
#include <iostream>

FMODSoundManager& FMODSoundManager::Instance()
{
    static FMODSoundManager instance;
    return instance;
}

void FMODSoundManager::Initialize()
{
    FMOD_RESULT result = FMOD::System_Create(&system_);
    if (result != FMOD_OK)
    {
        std::cerr << "FMOD System creation failed: " << FMOD_ErrorString(result) << std::endl;
        return;
    }
    result = system_->init(512, FMOD_INIT_NORMAL, nullptr);
    if (result != FMOD_OK)
    {
        std::cerr << "FMOD System init failed: " << FMOD_ErrorString(result) << std::endl;
    }
}

void FMODSoundManager::Release()
{
    for (auto& [_, sound] : sounds_)
    {
        if (sound)
        {
            sound->release();
        }
    }
    sounds_.clear();

    if (system_)
    {
        system_->close();
        system_->release();
        system_ = nullptr;
    }
}

void FMODSoundManager::LoadSound(const std::string& name, const std::string& path, bool loop)
{
    if (!system_) return;

    FMOD_MODE mode = loop ? FMOD_LOOP_NORMAL : FMOD_DEFAULT;
    FMOD::Sound* sound = nullptr;
    FMOD_RESULT result = system_->createSound(path.c_str(), mode, nullptr, &sound);
    if (result == FMOD_OK && sound)
    {
        sounds_[name] = sound;
    }
    else
    {
        std::cerr << "Failed to load sound: " << path << " - " << FMOD_ErrorString(result) << std::endl;
    }
}

void FMODSoundManager::PlaySound(const std::string& name, bool loop, float volume)
{
    if (!system_) return;
    auto it = sounds_.find(name);
    if (it == sounds_.end()) return;

    FMOD::Channel* channel = nullptr;
    FMOD_RESULT result = system_->playSound(it->second, nullptr, false, &channel);
    if (result == FMOD_OK && channel)
    {
        channel->setVolume(volume);
        channels_[name] = channel;
    }
}

void FMODSoundManager::StopSound(const std::string& name)
{
    auto it = channels_.find(name);
    if (it != channels_.end() && it->second)
    {
        it->second->stop();
    }
}
