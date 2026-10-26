#pragma once
#include "AnimationState.h"

enum class SuperDragonAnimationTrack { kFlyCastSpell, kFlyIdle, kFlyDie, kFlyTakeDamage, 
	kFlyUpFast, kFlyRightFast, kFlyDownFast, kFlyBiteAttackLow, kFlyFireBreathAttackLow };


class SuperDragonAnimationState :
    public AnimationState
{
public:
	SuperDragonAnimationState();
	virtual ~SuperDragonAnimationState() {}

	virtual void Enter(int animation_track, std::shared_ptr<Object> object, std::shared_ptr<AnimatorComponent> animator) override;
	virtual int Run(float elapsed_time, std::shared_ptr<Object> object, bool is_end, std::shared_ptr<AnimatorComponent> animator) override;
	virtual void Exit(int animation_track, std::shared_ptr<Object> object, std::shared_ptr<AnimatorComponent> animator) override;

	virtual AnimationState* GetCopy() override;

	virtual int GetDeadAnimationTrack() const override;

private:
	float attack_time_ = 0.f; 
	bool is_attack_ = false; // 공격 중인지 여부
};

