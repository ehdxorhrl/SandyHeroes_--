#include "stdafx.h"
#include "StrongDragonAnimationState.h"
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
	}

}

int StrongDragonAnimationState::Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)
{
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

}

AnimationState* StrongDragonAnimationState::GetCopy()
{
    return new StrongDragonAnimationState(*this);
}

int StrongDragonAnimationState::GetDeadAnimationTrack() const
{
    return (int)StrongDragonAnimationTrack::kDie;
}
