#pragma once
#include "AnimationState.h"

enum class ShotDragonAnimationTrack { kIdle, kRun, kDie, kAttack };

class ShotDragonAnimationState :
    public AnimationState
{
public:
	ShotDragonAnimationState() {}
	virtual ~ShotDragonAnimationState() {}

	virtual void Enter(int animation_track, Object* object, AnimatorComponent* animator) override;
	virtual int Run(Object* object, bool is_end, AnimatorComponent* animator) override;
	virtual void Exit(int animation_track, Object* object, AnimatorComponent* animator) override;

	virtual AnimationState* GetCopy() override;

	virtual int GetDeadAnimationTrack() const override;

};

