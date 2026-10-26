#pragma once
#include "AnimationState.h"

enum class ChestAnimationTrack { kClose, kCloseToOpen, kOpen, kOpenToClose };

class ChestAnimationState :
    public AnimationState
{
public:
	ChestAnimationState();
	virtual ~ChestAnimationState() {}

	virtual void Enter(int animation_track, std::shared_ptr<Object> object, std::shared_ptr<AnimatorComponent> animator) override;
	virtual int Run(float elapsed_time, std::shared_ptr<Object> object, bool is_end, std::shared_ptr<AnimatorComponent> animator) override;
	virtual void Exit(int animation_track, std::shared_ptr<Object> object, std::shared_ptr<AnimatorComponent> animator) override;

	virtual AnimationState* GetCopy() override;

	virtual int GetDeadAnimationTrack() const override;


    
};

