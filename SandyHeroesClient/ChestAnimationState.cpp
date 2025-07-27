#include "stdafx.h"
#include "ChestAnimationState.h"
#include "AnimationTrack.h"
#include "Object.h"

ChestAnimationState::ChestAnimationState()
{
    set_animation_track((int)ChestAnimationTrack::kClose);

}

void ChestAnimationState::Enter(int animation_track, Object* object, AnimatorComponent* animator)
{
	if ((int)ChestAnimationTrack::kClose == animation_track)
	{
		animation_loop_type_ = (int)AnimationLoopType::kLoop;
	}
	else if ((int)ChestAnimationTrack::kCloseToOpen == animation_track)
	{
		animation_loop_type_ = (int)AnimationLoopType::kOnce;
	}
	else if ((int)ChestAnimationTrack::kOpen == animation_track)
	{
		animation_loop_type_ = (int)AnimationLoopType::kLoop;
	}
	else if ((int)ChestAnimationTrack::kOpenToClose == animation_track)
	{
		animation_loop_type_ = (int)AnimationLoopType::kOnce;
	}
}

int ChestAnimationState::Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)
{
	switch ((ChestAnimationTrack)animation_track())
	{
		case ChestAnimationTrack::kClose:

		break;
		case ChestAnimationTrack::kCloseToOpen:
		if (is_end)
		{
			ChangeAnimationTrack((int)ChestAnimationTrack::kOpen, object, animator);
		}
		break;
		case ChestAnimationTrack::kOpen:

		break;
		case ChestAnimationTrack::kOpenToClose:
			if (is_end)
			{
				//object->set_is_dead(true);
			}
		break;
	default:
		break;
	}	
	return animation_track();
}

void ChestAnimationState::Exit(int animation_track, Object* object, AnimatorComponent* animator)
{

}

AnimationState* ChestAnimationState::GetCopy()
{
    return new ChestAnimationState(*this);
}

int ChestAnimationState::GetDeadAnimationTrack() const
{
    return (int)ChestAnimationTrack::kOpenToClose;
}
