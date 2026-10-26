#include "stdafx.h"
#include "HitDragonAnimationState.h"
#include "Object.h"
#include "MovementComponent.h"
#include "FMODSoundManager.h"
#include "GameFramework.h"
#include "Scene.h"

HitDragonAnimationState::HitDragonAnimationState()
{
	set_animation_track((int)HitDragonAnimationTrack::kIdle);
}

void HitDragonAnimationState::Enter(int animation_track, std::shared_ptr<Object> object, std::shared_ptr<AnimatorComponent> animator)
{
	if ((int)HitDragonAnimationTrack::kSlashLeftAttack == animation_track)
	{
		FMODSoundManager::Instance().PlaySound("punch", false, 0.3f);
	}
}

int HitDragonAnimationState::Run(float elapsed_time, std::shared_ptr<Object> object, bool is_end, std::shared_ptr<AnimatorComponent> animator)
{
	switch ((HitDragonAnimationTrack)animation_track())
	{
	case HitDragonAnimationTrack::kDie:
		if (is_end)
		{
			GameFramework::Instance()->scene()->DeleteObject(object);
		}
		break;
	case HitDragonAnimationTrack::kSlashLeftAttack:

		if (is_end)
		{
			ChangeAnimationTrack((int)HitDragonAnimationTrack::kIdle, object, animator);
		}
		break;

	default:
		break;
	}
    return animation_track();
}

void HitDragonAnimationState::Exit(int animation_track, std::shared_ptr<Object> object, std::shared_ptr<AnimatorComponent> animator)
{
	if (animation_track == (int)HitDragonAnimationTrack::kSlashLeftAttack)
	{
		animation_loop_type_ = 0;
	}
}

AnimationState* HitDragonAnimationState::GetCopy()
{
    return new HitDragonAnimationState(*this);
}

int HitDragonAnimationState::GetDeadAnimationTrack() const
{
	return (int)HitDragonAnimationTrack::kDie;
}
