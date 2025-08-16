#include "stdafx.h"
#include "SuperDragonAnimationState.h"
#include "Object.h"

SuperDragonAnimationState::SuperDragonAnimationState()
{
	set_animation_track((int)SuperDragonAnimationTrack::kFlyCastSpell);
	animation_loop_type_ = 1; //Once

}

void SuperDragonAnimationState::Enter(int animation_track, Object* object, AnimatorComponent* animator)
{
	switch ((SuperDragonAnimationTrack)animation_track)
	{
	case SuperDragonAnimationTrack::kFlyCastSpell:
	case SuperDragonAnimationTrack::kFlyTakeDamage:	
	case SuperDragonAnimationTrack::kFlyDie:
		animation_loop_type_ = 1; //Once
		break;
	case SuperDragonAnimationTrack::kFlyIdle:
		animation_loop_type_ = 0; //loop
		break;
	default:
		break;
	}

}

int SuperDragonAnimationState::Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)
{
	switch ((SuperDragonAnimationTrack)animation_track())
	{
	case SuperDragonAnimationTrack::kFlyCastSpell:
		if (is_end)
		{
			ChangeAnimationTrack((int)SuperDragonAnimationTrack::kFlyIdle, object, animator);
		}
		break;
	case SuperDragonAnimationTrack::kFlyDie:
		if (is_end)
		{
			object->set_is_dead(true);
		}
		break;
	case SuperDragonAnimationTrack::kFlyBiteAttackLow:
		if (is_end)
		{
			ChangeAnimationTrack((int)SuperDragonAnimationTrack::kFlyIdle, object, animator);
		}
		break;

	default:
		break;
	}
	return animation_track();
}

void SuperDragonAnimationState::Exit(int animation_track, Object* object, AnimatorComponent* animator)
{
	if((SuperDragonAnimationTrack)animation_track == SuperDragonAnimationTrack::kFlyBiteAttackLow)
	{
		animation_loop_type_ = 0; //loop
	}
}

AnimationState* SuperDragonAnimationState::GetCopy()
{
	return new SuperDragonAnimationState(*this);
}

int SuperDragonAnimationState::GetDeadAnimationTrack() const
{
	return (int)SuperDragonAnimationTrack::kFlyDie;
}
