#include "stdafx.h"
#include "StrongDragonAnimationState.h"
#include "Object.h"
#include "AnimatorComponent.h"
#include "FMODSoundManager.h"
#include "GameFramework.h"
#include "Scene.h"

StrongDragonAnimationState::StrongDragonAnimationState()
{
	set_animation_track((int)StrongDragonAnimationTrack::kSpawn);
	animation_loop_type_ = 1; //Once

}

StrongDragonAnimationState::~StrongDragonAnimationState()
{
	Exit(animation_track(), nullptr, nullptr);
}

void StrongDragonAnimationState::Enter(int animation_track, std::shared_ptr<Object> object, std::shared_ptr<AnimatorComponent> animator)
{
	if ((int)StrongDragonAnimationTrack::kSpawn == animation_track)
	{
		animation_loop_type_ = 1; //Once
	}
	if ((int)StrongDragonAnimationTrack::kSpinAttackOnce == animation_track)
	{
		animation_loop_type_ = 1; //Once
		FMODSoundManager::Instance().PlaySound("spin", false, 0.3f);
	}
	if ((int)StrongDragonAnimationTrack::kSpinAttackLoop == animation_track)
	{
		FMODSoundManager::Instance().PlaySound("spin_loop", true, 0.3f);
	}
}

int StrongDragonAnimationState::Run(float elapsed_time, std::shared_ptr<Object> object, bool is_end, std::shared_ptr<AnimatorComponent> animator)
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
			GameFramework::Instance()->scene()->DeleteObject(object);
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

void StrongDragonAnimationState::Exit(int animation_track, std::shared_ptr<Object> object, std::shared_ptr<AnimatorComponent> animator)
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
		FMODSoundManager::Instance().StopSound("spin_loop");
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
