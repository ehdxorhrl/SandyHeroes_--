#include "stdafx.h"
#include "BombDragonAnimationState.h"
#include "MovementComponent.h"
#include "Object.h"
#include "AnimatorComponent.h"

BombDragonAnimationState::BombDragonAnimationState()
{
	set_animation_track((int)BombDragonAnimationTrack::kSpawn);
	animation_loop_type_ = 1; //Once
}

void BombDragonAnimationState::Enter(int animation_track, Object* object, AnimatorComponent* animator)
{
	if ((int)BombDragonAnimationTrack::kSpawn == animation_track)
	{
		animation_loop_type_ = 1; //Once
	}
	if ((int)BombDragonAnimationTrack::kExplode == animation_track)
	{
		//TODO: 터져용 폭발 사운드 재생 1회
		//TODO: 폭발 파티클 재생
		XMFLOAT3 particle_position = object->FindFrame("RigHub")->world_position_vector();
	}

}

int BombDragonAnimationState::Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)
{
	auto movement = Object::GetComponentInChildren<MovementComponent>(object);
	auto velocity_xz = movement->velocity();
	velocity_xz.y = 0.f;
	float speed = xmath_util_float3::Length(velocity_xz);

	switch ((BombDragonAnimationTrack)animation_track())
	{
	case BombDragonAnimationTrack::kSpawn:
		if (is_end)
		{
			ChangeAnimationTrack((int)BombDragonAnimationTrack::kIdle, object, animator);
		}
		break;
	case BombDragonAnimationTrack::kIdle:
		if (speed > 0.f)
		{
			ChangeAnimationTrack((int)BombDragonAnimationTrack::kRun, object, animator);
		}
		break;
	case BombDragonAnimationTrack::kRun:
		if (IsZero(speed))
		{
			ChangeAnimationTrack((int)BombDragonAnimationTrack::kIdle, object, animator);
		}
		break;
	case BombDragonAnimationTrack::kDie:
		if (is_end)
		{
			object->set_is_dead(true);
		}
		break;
	case BombDragonAnimationTrack::kExplode:
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

void BombDragonAnimationState::Exit(int animation_track, Object* object, AnimatorComponent* animator)
{
	if ((int)BombDragonAnimationTrack::kSpawn == animation_track)
	{
		animation_loop_type_ = 0; //Loop
	}

}

AnimationState* BombDragonAnimationState::GetCopy()
{
    return new BombDragonAnimationState(*this);
}

int BombDragonAnimationState::GetDeadAnimationTrack() const
{
    return (int)BombDragonAnimationTrack::kDie;
}
