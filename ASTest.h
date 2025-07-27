#pragma once
#include "AnimationState.h"

enum class TestAnimationTrack{kIdle, kRun};

class ASTest :
    public AnimationState
{
public:
    virtual int Run(Object* object, bool is_end) override;
    virtual AnimationState* GetCopy() override;

private:
    TestAnimationTrack animation_track_ = TestAnimationTrack::kIdle;
};

