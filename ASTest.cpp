#include "stdafx.h"
#include "ASTest.h"
#include "Object.h"

int ASTest::Run(Object* object, bool is_end)
{
	switch (animation_track_)
	{
	case TestAnimationTrack::kIdle:
		if (xmath_util_float3::Length(object->velocity()) > 0)
		{
			animation_track_ = TestAnimationTrack::kRun;
		}
		break;
	case TestAnimationTrack::kRun:
		if (IsZero(xmath_util_float3::Length(object->velocity())))
		{
			animation_track_ = TestAnimationTrack::kIdle;
		}
		break;
	default:
		break;
	}

	return (int)animation_track_;
}

AnimationState* ASTest::GetCopy()
{
	return new ASTest(*this);
}
