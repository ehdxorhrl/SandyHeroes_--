#include "stdafx.h"
#include "ShotDragonAnimationState.h"
#include "Object.h"
#include "MovementComponent.h"

void ShotDragonAnimationState::Enter(int animation_track, Object* object, AnimatorComponent* animator)
{
	if ((int)ShotDragonAnimationTrack::kAttack == animation_track)
	{
		//TODO: 쏴용 공격 사운드 재생 1회
	}
}

int ShotDragonAnimationState::Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)
{
	auto movement = Object::GetComponentInChildren<MovementComponent>(object);
	auto velocity_xz = movement->velocity();
	velocity_xz.y = 0.f;
	float speed = xmath_util_float3::Length(velocity_xz);

	switch ((ShotDragonAnimationTrack)animation_track())
	{
	case ShotDragonAnimationTrack::kIdle:
		if (speed > 0.f)
		{
			ChangeAnimationTrack((int)ShotDragonAnimationTrack::kRun, object, animator);
		}
		break;
	case ShotDragonAnimationTrack::kRun:
		if (IsZero(speed))
		{
			ChangeAnimationTrack((int)ShotDragonAnimationTrack::kIdle, object, animator);
		}
		break;
	case ShotDragonAnimationTrack::kDie:
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

void ShotDragonAnimationState::Exit(int animation_track, Object* object, AnimatorComponent* animator)
{
}

AnimationState* ShotDragonAnimationState::GetCopy()
{
    return new ShotDragonAnimationState(*this);
}

int ShotDragonAnimationState::GetDeadAnimationTrack() const
{
    return (int)ShotDragonAnimationTrack::kDie;
}
