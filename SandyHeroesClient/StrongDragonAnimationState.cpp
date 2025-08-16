#include "stdafx.h"
#include "StrongDragonAnimationState.h"
#include "MovementComponent.h"
#include "Object.h"
#include "AnimatorComponent.h"

StrongDragonAnimationState::StrongDragonAnimationState()
{
	set_animation_track((int)StrongDragonAnimationTrack::kSpawn);
	animation_loop_type_ = 1; //Once

}

void StrongDragonAnimationState::Enter(int animation_track, Object* object, AnimatorComponent* animator)
{
	if ((int)StrongDragonAnimationTrack::kSpawn == animation_track)
	{
		animation_loop_type_ = 1; //Once
	}
	if ((int)StrongDragonAnimationTrack::kSpinAttackOnce == animation_track)
	{
		animation_loop_type_ = 1; //Once
		//TODO: 쎄용 공격 사운드 재생 1회
	}
	if ((int)StrongDragonAnimationTrack::kSpinAttackLoop == animation_track)
	{
		//TODO: 쎄용 공격 사운드 재생 Loop

	}
}

int StrongDragonAnimationState::Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)
{
	auto movement = Object::GetComponentInChildren<MovementComponent>(object);
	auto velocity_xz = movement->velocity();
	velocity_xz.y = 0.f;
	float speed = xmath_util_float3::Length(velocity_xz);



	switch ((StrongDragonAnimationTrack)animation_track())
	{
	case StrongDragonAnimationTrack::kSpawn:
		if (is_end)
		{
			ChangeAnimationTrack((int)StrongDragonAnimationTrack::kIdle, object, animator);
		}
		break;
	case StrongDragonAnimationTrack::kIdle:
		break;
	case StrongDragonAnimationTrack::kDie:
		if (is_end)
		{
			object->set_is_dead(true);
		}
		break;
	case StrongDragonAnimationTrack::kSpinAttackOnce:
		if (is_end)
		{
			ChangeAnimationTrack((int)StrongDragonAnimationTrack::kIdle, object, animator);
		}
		break;
	default:
		break;
	}
	return animation_track();
}

void StrongDragonAnimationState::Exit(int animation_track, Object* object, AnimatorComponent* animator)
{
	if ((int)StrongDragonAnimationTrack::kSpawn == animation_track)
	{
		animation_loop_type_ = 0; 
	}
	if ((int)StrongDragonAnimationTrack::kSpinAttackOnce == animation_track)
	{
		animation_loop_type_ = 0; 
	}
	if ((int)StrongDragonAnimationTrack::kSpinAttackLoop == animation_track)
	{
		//TODO: 쎄용 공격 사운드 재생 Loop 종료

	}
}

AnimationState* StrongDragonAnimationState::GetCopy()
{
    return new StrongDragonAnimationState(*this);
}

int StrongDragonAnimationState::GetDeadAnimationTrack() const
{
    return (int)StrongDragonAnimationTrack::kDie;
}
