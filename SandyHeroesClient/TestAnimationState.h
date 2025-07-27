#pragma once
#include "AnimationState.h"
class TestAnimationState :
    public AnimationState
{
public:
	virtual void Enter(int animation_track, Object* object, AnimatorComponent* animator) override;
	virtual int Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator) override;
	virtual void Exit(int animation_track, Object* object, AnimatorComponent* animator) override;

	virtual AnimationState* GetCopy() override;
private:
	float time_{ 0.f };
	int track_index_{ 4 };
};

