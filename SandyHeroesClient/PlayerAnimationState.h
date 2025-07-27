#pragma once
#include "AnimationState.h"

enum class PlayerAnimationTrack{ kIdle, kRun, kJump, kDash };

class PlayerAnimationState :
    public AnimationState
{
public:
	virtual ~PlayerAnimationState() {}

	//애니메이션 트랙 변경시 해당 트랙에 1번만 필요한 설정을 하는 함수
	virtual void Enter(int animation_track, Object* object, AnimatorComponent* animator) override;
	virtual int Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator) override;
	virtual void Exit(int animation_track, Object* object, AnimatorComponent* animator) override;

	virtual AnimationState* GetCopy() override;
	
private:
	//총기 손 모션에 따라 다른 애니메이션을 실행하기 위한 offset
	static const int kGunTypeOffset = 3;
};

