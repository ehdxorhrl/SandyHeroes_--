#include "stdafx.h"
#include "AnimationState.h"
#include "Object.h"
#include "AnimatorComponent.h"

int AnimationState::GetDeadAnimationTrack() const
{
	// 죽는 애니메이션이 없으면 -1을 리턴
	return -1;
}

void AnimationState::ChangeAnimationTrack(int animation_track, Object* object, AnimatorComponent* animator)
{
	Exit(animation_track_, object, animator);
	Enter(animation_track, object, animator);
	animation_track_ = animation_track;
}

void AnimationState::set_animation_track(int value)
{
	animation_track_ = value;
}

void AnimationState::set_animation_loop_type(int value)
{
	animation_loop_type_ = value;
}

int AnimationState::animation_loop_type() const
{
	return animation_loop_type_;
}

int AnimationState::animation_track() const
{
	return animation_track_;
}
