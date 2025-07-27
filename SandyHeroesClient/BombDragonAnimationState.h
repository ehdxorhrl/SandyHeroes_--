#pragma once
#include "AnimationState.h"

enum class BombDragonAnimationTrack { kIdle, kDie, kGoingToExplode, kExplode, kSpawn, kRun };

class BombDragonAnimationState :
    public AnimationState
{
public:
	BombDragonAnimationState();
	virtual ~BombDragonAnimationState() {}

	virtual void Enter(int animation_track, Object* object, AnimatorComponent* animator) override;
	virtual int Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator) override;
	virtual void Exit(int animation_track, Object* object, AnimatorComponent* animator) override;

	virtual AnimationState* GetCopy() override;

	virtual int GetDeadAnimationTrack() const override;



};

