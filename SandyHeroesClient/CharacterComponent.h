#pragma once
#include "Component.h"

class AnimatorComponent;
class MovementComponent;

class CharacterComponent :
    public Component
{
public:
    DECLARE_COMPONENT(CharacterComponent, Component)

    CharacterComponent() = default;
	CharacterComponent(Object* owner) : Component(owner) {}
	CharacterComponent(const std::shared_ptr<Object>& owner) : Component(owner) {}
    virtual ~CharacterComponent() = default;

    virtual Component* GetCopy() = 0;

    virtual void Update(float elapsed_time) = 0;

	virtual void HendleCollision(std::shared_ptr<Object> other_object) = 0;

protected:
	std::weak_ptr<AnimatorComponent> animator_{}; 
	std::weak_ptr<MovementComponent> movement_{}; 

};

