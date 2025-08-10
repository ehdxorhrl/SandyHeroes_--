#include "SoundComponent.h"
#include "FMODSoundManager.h"

SoundComponent::SoundComponent(Object* owner) : Component(owner) {}

Component* SoundComponent::GetCopy()
{
    return new SoundComponent(*this);
}

void SoundComponent::Load(const std::string& name, const std::string& path, bool loop)
{
    FMODSoundManager::Instance().LoadSound(name, path, loop);
}

void SoundComponent::Play(const std::string& name, float volume)
{
    FMODSoundManager::Instance().PlaySound(name, false, volume);
}

void SoundComponent::Stop(const std::string& name)
{
    FMODSoundManager::Instance().StopSound(name);
}