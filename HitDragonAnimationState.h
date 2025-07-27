#pragma once
#include "AnimationState.h"

enum class HitDragonAnimationTrack { kBreathAttack, kCastSpell, kDashAttack, kDie, kIdle, kJumpSmashAttack, kRun, kSlashLeftAttack, kSlashRightAttack, kTakeDamage };


class HitDragonAnimationState :
    public AnimationState
{
public:
	HitDragonAnimationState();
	virtual ~HitDragonAnimationState() {}

	virtual void Enter(int animation_track, Object* object, AnimatorComponent* animator) override;
	virtual int Run(Object* object, bool is_end, AnimatorComponent* animator) override;
	virtual void Exit(int animation_track, Object* object, AnimatorComponent* animator) override;

	virtual AnimationState* GetCopy() override;

	virtual int GetDeadAnimationTrack() const override;

};

