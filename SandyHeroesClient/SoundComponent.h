#pragma once
#include "Component.h"
#include <string>

class SoundComponent : public Component
{
public:
    SoundComponent(Object* owner);
    virtual ~SoundComponent() = default;

    virtual Component* GetCopy() override;

    void Load(const std::string& name, const std::string& path, bool loop = false);
    void Play(const std::string& name, float volume = 1.0f);
    void Stop(const std::string& name);
};