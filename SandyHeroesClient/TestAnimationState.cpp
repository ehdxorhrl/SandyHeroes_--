#include "stdafx.h"
#include "TestAnimationState.h"

void TestAnimationState::Enter(int animation_track, std::shared_ptr<Object> object, std::shared_ptr<AnimatorComponent> animator)
{
}

int TestAnimationState::Run(float elapsed_time, std::shared_ptr<Object> object, bool is_end, std::shared_ptr<AnimatorComponent> animator)
{
	time_ += elapsed_time;
	if (time_ > 2.f)
	{
		time_ = 0.f;
		if (track_index_ == 4)
			track_index_ = 6;
		else
			track_index_ = 4;
	}
    return track_index_;
}

void TestAnimationState::Exit(int animation_track, std::shared_ptr<Object> object, std::shared_ptr<AnimatorComponent> animator)
{

}

AnimationState* TestAnimationState::GetCopy()
{
    return nullptr;
}
