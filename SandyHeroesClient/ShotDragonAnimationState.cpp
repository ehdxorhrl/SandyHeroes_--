#include "stdafx.h"
#include "ShotDragonAnimationState.h"
#include "Object.h"
#include "MovementComponent.h"
#include "FMODSoundManager.h"

void ShotDragonAnimationState::Enter(int animation_track, Object* object, AnimatorComponent* animator)
{
	if ((int)ShotDragonAnimationTrack::kAttack == animation_track)
	{
		FMODSoundManager::Instance().PlaySound("shot", false, 0.3f);
	}
}

int ShotDragonAnimationState::Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)
{

	switch ((ShotDragonAnimationTrack)animation_track())
	{
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
