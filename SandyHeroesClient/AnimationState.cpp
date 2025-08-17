#include "stdafx.h"
#include "AnimationState.h"
#include "Object.h"
#include "AnimatorComponent.h"

AnimationState::~AnimationState()
{
}

int AnimationState::GetDeadAnimationTrack() const
{
	// 죽는 애니메이션이 없으면 -1을 리턴
	return -1;
}

void AnimationState::ChangeAnimationTrack(int animation_track, Object* object, AnimatorComponent* animator)
{
	if (animation_track_ == animation_track)
	{
		return; // 현재 애니메이션 트랙과 변경하려는 트랙이 같으면 아무것도 하지 않음
	}
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
