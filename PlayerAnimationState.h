#pragma once
#include "AnimationState.h"

enum class PlayerAnimationTrack{ kIdle, kRun, kRunL, kRunR, kJump, kDash };

class PlayerAnimationState :
    public AnimationState
{
public:
	virtual ~PlayerAnimationState() {}

	virtual int Run(Object* object, bool is_end) override;
	virtual AnimationState* GetCopy() override;
	
};

