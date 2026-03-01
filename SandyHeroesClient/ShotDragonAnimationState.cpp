#include "stdafx.h"
#include "ShotDragonAnimationState.h"
#include "Object.h"
#include "MovementComponent.h"
#include "FMODSoundManager.h"
#include "GameFramework.h"
#include "Scene.h"

void ShotDragonAnimationState::Enter(int animation_track, std::shared_ptr<Object> object, std::shared_ptr<AnimatorComponent> animator)
{
	if ((int)ShotDragonAnimationTrack::kAttack == animation_track)
	{
		FMODSoundManager::Instance().PlaySound("shot", false, 0.3f);
	}
}

int ShotDragonAnimationState::Run(float elapsed_time, std::shared_ptr<Object> object, bool is_end, std::shared_ptr<AnimatorComponent> animator)
{

	switch ((ShotDragonAnimationTrack)animation_track())
	{
	case ShotDragonAnimationTrack::kDie:
		if (is_end)
		{
			GameFramework::Instance()->scene()->DeleteObject(object);
		}
		break;
	default:
		break;
	}
	return animation_track();
}

void ShotDragonAnimationState::Exit(int animation_track, std::shared_ptr<Object> object, std::shared_ptr<AnimatorComponent> animator)
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
