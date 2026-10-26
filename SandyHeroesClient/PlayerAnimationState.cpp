#include "stdafx.h"
#include "PlayerAnimationState.h"
#include "Object.h"
#include "MovementComponent.h"
#include "AnimatorComponent.h"

void PlayerAnimationState::Enter(int animation_track, std::shared_ptr<Object> object, std::shared_ptr<AnimatorComponent> animator)
{
	animator->set_is_ignore_root_bone_traslation(true);
}

int PlayerAnimationState::Run(float elapsed_time, std::shared_ptr<Object> object, bool is_end, std::shared_ptr<AnimatorComponent> animator)
{

	switch ((PlayerAnimationTrack)animation_track())
	{
	case PlayerAnimationTrack::kRun:	
		if (is_end)
		{
			ChangeAnimationTrack((int)PlayerAnimationTrack::kIdle, object, animator);
			set_animation_loop_type(0);
		}
		break;
	default:
		break;
	}
	return animation_track();

}

void PlayerAnimationState::Exit(int animation_track, std::shared_ptr<Object> object, std::shared_ptr<AnimatorComponent> animator)
{
	animator->set_is_ignore_root_bone_traslation(false);
}

AnimationState* PlayerAnimationState::GetCopy()
{
    return new PlayerAnimationState(*this);
}
