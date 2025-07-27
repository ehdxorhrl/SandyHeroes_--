#include "stdafx.h"
#include "HitDragonAnimationState.h"
#include "Object.h"
#include "MovementComponent.h"

HitDragonAnimationState::HitDragonAnimationState()
{
	set_animation_track((int)HitDragonAnimationTrack::kIdle);
}

void HitDragonAnimationState::Enter(int animation_track, Object* object, AnimatorComponent* animator)
{
}

int HitDragonAnimationState::Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)
{
	auto movement = Object::GetComponentInChildren<MovementComponent>(object);
	auto velocity_xz = movement->velocity();
	velocity_xz.y = 0.f;
	float speed = xmath_util_float3::Length(velocity_xz);

	switch ((HitDragonAnimationTrack)animation_track())
	{
	case HitDragonAnimationTrack::kIdle:
		if (speed > 0.f)
		{
			ChangeAnimationTrack((int)HitDragonAnimationTrack::kRun, object, animator);
		}
		break;
	case HitDragonAnimationTrack::kRun:
		if (IsZero(speed))
		{
			ChangeAnimationTrack((int)HitDragonAnimationTrack::kIdle, object, animator);
		}
		break;
	case HitDragonAnimationTrack::kDie:
		if (is_end)
		{
			object->set_is_dead(true);
		}
		break;
	default:
		break;
	}
    return animation_track();
}

void HitDragonAnimationState::Exit(int animation_track, Object* object, AnimatorComponent* animator)
{
}

AnimationState* HitDragonAnimationState::GetCopy()
{
    return new HitDragonAnimationState(*this);
}

int HitDragonAnimationState::GetDeadAnimationTrack() const
{
	return (int)HitDragonAnimationTrack::kDie;
}
