#include "stdafx.h"
#include "MonsterComponent.h"
#include "Object.h"
#include "AnimatorComponent.h"
#include "AnimationState.h"
#include "MovementComponent.h"
#include "UiMeshComponent.h"
#include "ShotDragonAnimationState.h"
#include "FMODSoundManager.h"

MonsterComponent::MonsterComponent(Object* owner) : Component(owner)
{
}
MonsterComponent::MonsterComponent(const std::shared_ptr<Object>& owner) : Component(owner)
{
}

MonsterComponent::MonsterComponent(const MonsterComponent& other) : Component(other),
shield_(other.shield_), hp_(other.hp_), 
attack_force_(other.attack_force_), monster_type_(other.monster_type_), 
target_(other.target_), scene_(other.scene_)
{
}

Component* MonsterComponent::GetCopy()
{
    return new MonsterComponent(*this);
}

void MonsterComponent::Update(float elapsed_time)
{
    if (is_dead_animationing_)
    {
        return;
    }

    if (hp_ <= 0)
    {
		auto owner = owner_.lock();
        if(!owner)
        {
            return;
		}
        auto animator = Object::GetComponentInChildren<AnimatorComponent>(owner);
        if (!animator)
        {
            std::string temp = owner->name() + "�� MonsterComponent ���� �ִϸ��̼� ��� �������� ������ ������ϴ�.";
            std::wstring debug_str;
            debug_str.assign(temp.begin(), temp.end());

            OutputDebugString(debug_str.c_str());
            return;
        }
		auto animation_state = animator->animation_state();
        if (animation_state)
        {
            if (animation_state->GetDeadAnimationTrack() == -1)
            {
				scene_->DeleteObject(owner);
                return;
            }
			animation_state->ChangeAnimationTrack(animation_state->GetDeadAnimationTrack(), owner, animator);
			animation_state->set_animation_loop_type(1); // Once
            is_dead_animationing_ = true;
            return;
        }
    }
}

void MonsterComponent::HitDamage(float damage)
{
	if (shield_ > 0)
	{
		shield_ -= damage;
		if (shield_ < 0)
		{
			hp_ += shield_; // shield�� ������ hp�� ������
			shield_ = 0;
		}
        FMODSoundManager::Instance().PlaySound("hit", false, 0.3f);
	}
	else
	{
		hp_ -= damage;
        FMODSoundManager::Instance().PlaySound("hit", false, 0.3f);
	}
	if (hp_ < 0)
	{
		hp_ = 0;
	}
}

void MonsterComponent::ApplyStatusEffect(StatusEffectType type, float duration, float damage,
    bool flame_frenzy, bool acid_frenzy, bool electric_frenzy)
{
    status_effects_[type] = { duration, 0.f, damage , flame_frenzy, acid_frenzy, electric_frenzy };

}

void MonsterComponent::set_shield(float value)
{
    shield_ = value;
    if (shield_ > max_shield_)
    {
        max_shield_ = shield_;
	}
}

void MonsterComponent::set_hp(float value)
{
    hp_ = value;
    if(hp_ > max_hp_)
    {
        max_hp_ = hp_;
	}
}

void MonsterComponent::set_attack_force(float value)
{
    attack_force_ = value;
}

void MonsterComponent::set_target(std::shared_ptr<Object> target)
{
	target_ = target;
}

void MonsterComponent::set_is_pushed(bool is_pushed)
{
    is_pushed_ = is_pushed;
}

void MonsterComponent::set_push_timer(float value)
{
    push_timer_ = value;
}

float MonsterComponent::shield() const
{
    return shield_;
}

float MonsterComponent::hp() const
{
    return hp_;
}

float MonsterComponent::max_hp() const
{
    return max_hp_;
}

float MonsterComponent::max_shield() const
{
    return max_shield_;
}

float MonsterComponent::attack_force() const
{
    return attack_force_;
}

void MonsterComponent::set_scene(Scene* value)
{
    scene_ = value;
}

bool MonsterComponent::IsDead() const
{
    return hp_ <= 0;
}
