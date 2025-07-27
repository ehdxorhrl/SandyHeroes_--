#pragma once
#include "AnimationState.h"

enum class ChestAnimationTrack { kClose, kCloseToOpen, kOpen, kOpenToClose };

class ChestAnimationState :
    public AnimationState
{
public:
	ChestAnimationState();
	virtual ~ChestAnimationState() {}

	virtual void Enter(int animation_track, Object* object, AnimatorComponent* animator) override;
	virtual int Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator) override;
	virtual void Exit(int animation_track, Object* object, AnimatorComponent* animator) override;

	virtual AnimationState* GetCopy() override;

	virtual int GetDeadAnimationTrack() const override;


    
};

