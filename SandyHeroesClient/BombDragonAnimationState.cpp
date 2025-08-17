#include "stdafx.h"
#include "BombDragonAnimationState.h"
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
}

int BombDragonAnimationState::Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)
{

	switch ((BombDragonAnimationTrack)animation_track())
	{
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
