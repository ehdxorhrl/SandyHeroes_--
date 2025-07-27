#include "stdafx.h"
#include "MonsterComponent.h"
#include "Object.h"
#include "AnimatorComponent.h"
#include "AnimationState.h"
#include "MovementComponent.h"
#include "UiMeshComponent.h"
#include "ShotDragonAnimationState.h"

MonsterComponent::MonsterComponent(Object* owner) : Component(owner)
{
}

MonsterComponent::MonsterComponent(const MonsterComponent& other) : Component(other),
shield_(other.shield_), hp_(other.hp_), attack_force_(other.attack_force_), monster_type_(other.monster_type_), target_(other.target_)
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
        auto animator = Object::GetComponentInChildren<AnimatorComponent>(owner_);
        if (!animator)
        {
            std::string temp = owner_->name() + "의 MonsterComponent 죽음 애니메이션 출력 과정에서 문제가 생겼습니다.";
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
				// 죽는 애니메이션이 없으면 그냥 죽는다.
                owner_->set_is_dead(true);
                return;
            }
			// 죽는 애니메이션으로 전환
			animation_state->ChangeAnimationTrack(animation_state->GetDeadAnimationTrack(), owner_, animator);
			animation_state->set_animation_loop_type(1); // Once
            is_dead_animationing_ = true;
            return;
        }
    }

	//TODO: 서버에서 몬스터의 상태를 받아서 업데이트
	//EX) ai->Update(owner_, elapsed_time);
    if (target_)
    {
		auto movement = Object::GetComponentInChildren<MovementComponent>(owner_);
		if (movement)
		{
			XMFLOAT3 look = owner_->look_vector();
            look.y = 0.f;
			look = xmath_util_float3::Normalize(look);
			XMFLOAT3 direction = target_->world_position_vector() - owner_->world_position_vector();
			direction.y = 0.f;
			direction = xmath_util_float3::Normalize(direction);
			float angle = xmath_util_float3::AngleBetween(look, direction);
            if (angle > XM_PI / 180.f * 5.f)
            {
                //회전 방향 연산
				XMFLOAT3 cross = xmath_util_float3::CrossProduct(look, direction);
				if (cross.y < 0)
				{
					angle = -angle;
				}
				angle = XMConvertToDegrees(angle);
				owner_->Rotate(0.f, angle, 0.f);
            }
            if (owner_->tag() == "Shot_Dragon")
            {
                auto animator = Object::GetComponent<AnimatorComponent>(owner_);
                auto animation_state = animator->animation_state();
                animation_state->ChangeAnimationTrack((int)ShotDragonAnimationTrack::kAttack, owner_, animator);
                animation_state->set_animation_loop_type(0); // Loop

                return;
            }
            if (owner_->tag() == "Strong_Dragon")
            {
                return;
            }

			movement->MoveXZ(direction.x, direction.z, 5.f);
            movement->set_max_speed_xz(3.5f);
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
			hp_ += shield_; // shield가 음수면 hp에 더해줌
			shield_ = 0;
		}
	}
	else
	{
		hp_ -= damage;
	}
	if (hp_ < 0)
	{
		hp_ = 0;
	}
}

void MonsterComponent::set_shield(float value)
{
    shield_ = value;
}

void MonsterComponent::set_hp(float value)
{
    hp_ = value;
}

void MonsterComponent::set_attack_force(float value)
{
    attack_force_ = value;
}

void MonsterComponent::set_target(Object* target)
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
}

bool MonsterComponent::IsDead() const
{
    return hp_ <= 0;
}
