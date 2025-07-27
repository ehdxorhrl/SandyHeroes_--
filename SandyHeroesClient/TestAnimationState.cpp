#include "TestAnimationState.h"

void TestAnimationState::Enter(int animation_track, Object* object, AnimatorComponent* animator)
{
}

int TestAnimationState::Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)
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

void TestAnimationState::Exit(int animation_track, Object* object, AnimatorComponent* animator)
{

}

AnimationState* TestAnimationState::GetCopy()
{
    return nullptr;
}
