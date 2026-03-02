#include "stdafx.h"
#include "ChestComponent.h"
#include "AnimatorComponent.h"
#include "ChestAnimationState.h"
#include "Object.h"
#include "MovementComponent.h"
#include "ParticleComponent.h"
#include "ScrollComponent.h"
#include "Scene.h"
#include "FMODSoundManager.h"
#include "GameFramework.h"

ChestComponent::ChestComponent(Object* owner, Scene* scene)
	: CharacterComponent(owner), is_open_(false), scene_(scene)
{
}

Component* ChestComponent::GetCopy()
{
    return new ChestComponent(*this);
}

void ChestComponent::Update(float elapsed_time)
{
}

void ChestComponent::HendleCollision(std::shared_ptr<Object> other_object)
{
	if (is_open_)
	{
		return;
	}

	is_open_ = true;
	
	auto locked_owner = owner_.lock();
	if (!locked_owner) return;

	auto locked_animator = animator_.lock();
    if (!locked_animator)
    {
		locked_animator = Object::GetComponent<AnimatorComponent>(locked_owner);
		animator_ = locked_animator;
        if (!locked_animator)
        {
            OutputDebugString(L"ChestComponent: animator_ is nullptr!");
			return;
        }
    }   

	locked_animator->animation_state()->ChangeAnimationTrack(
		(int)ChestAnimationTrack::kCloseToOpen, locked_owner, locked_animator);

	FMODSoundManager::Instance().PlaySound("chest", false, 0.3f);

	auto chest_particle = Object::GetComponent<ParticleComponent>(locked_owner);
	if (chest_particle)
	{
		chest_particle->set_loop(true);
	}

	std::shared_ptr<Object> scroll = scroll_model_->GetInstance();
	scroll->set_transform_matrix(locked_owner->transform_matrix() * scroll->transform_matrix());
	scroll->set_is_movable(true);
	auto scroll_comp = Object::GetComponent<ScrollComponent>(scroll);
	scroll_comp->set_is_active(true);
	scroll_comp->set_direction(XMFLOAT3(0.0f, 0.0007f, 0.0f));

	scroll_object_ = scroll;
	scene_->AddObject(scroll);
}

void ChestComponent::OpenChest(uint8_t scroll_type, ModelInfo* scroll_model)
{ 
	if (is_open_)
	{
		return;
	}

	is_open_ = true;

	auto locked_owner = owner_.lock();
	if (!locked_owner) return;

	auto locked_animator = animator_.lock();
	if (!locked_animator)
	{
		locked_animator = Object::GetComponent<AnimatorComponent>(locked_owner);
		animator_ = locked_animator;
		if (!locked_animator)
		{
			OutputDebugString(L"ChestComponent: animator_ is nullptr!");
			return;
		}
	}

	locked_animator->animation_state()->ChangeAnimationTrack(
		(int)ChestAnimationTrack::kCloseToOpen, locked_owner, locked_animator);

	FMODSoundManager::Instance().PlaySound("chest", false, 0.3f);

	auto chest_particle = Object::GetComponent<ParticleComponent>(locked_owner);
	if (chest_particle)
	{
		chest_particle->set_loop(true);
	}

	set_scroll_model(scroll_model);

	std::shared_ptr<Object> scroll = scroll_model_->GetInstance();
	scroll->set_transform_matrix(locked_owner->transform_matrix() * scroll->transform_matrix());
	scroll->set_is_movable(true);
	auto scroll_comp = Object::GetComponent<ScrollComponent>(scroll);
	scroll_comp->set_is_active(true);
	scroll_comp->set_direction(XMFLOAT3(0.0f, 0.0007f, 0.0f));
	scroll_comp->set_type(static_cast<ScrollType>(scroll_type));

	std::cout << "scroll_type: " << static_cast<int>(scroll_comp->type()) << '\n';

	scroll_object_ = scroll;
	scene_->AddObject(scroll);
}

ScrollType ChestComponent::TakeScroll()
{
	auto scroll_obj = scroll_object_.lock();
	if (!scroll_obj)
	{
		return ScrollType::None;
	}
	auto scroll_comp = Object::GetComponent<ScrollComponent>(scroll_obj);
	auto type = scroll_comp->type();

	std::cout << "scroll_type: " << static_cast<int>(scroll_comp->type()) << std::endl;

	GameFramework::Instance()->scene()->DeleteObject(scroll_obj);
	scroll_object_.reset();

	FMODSoundManager::Instance().PlaySound("scroll_pickup", true, 0.3f);

	auto locked_owner = owner_.lock();
	if(!locked_owner) return type;
	auto chest_particle = Object::GetComponent<ParticleComponent>(locked_owner);
	if (chest_particle)
	{
		chest_particle->set_loop(false);
	}

	return type;
}