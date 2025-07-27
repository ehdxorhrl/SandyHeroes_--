#include "stdafx.h"
#include "PlayerAnimationState.h"
#include "Object.h"

int PlayerAnimationState::Run(Object* object, bool is_end)
{
	constexpr float kPlayerDashSpeed = 70.f;
	XMFLOAT3 velocity_xz = object->velocity();
	velocity_xz.y = 0.f;

	switch ((PlayerAnimationTrack)animation_track_)
	{
	case PlayerAnimationTrack::kIdle:
	{
		if (xmath_util_float3::Length(velocity_xz) > 0)
		{
			animation_track_ = (int)PlayerAnimationTrack::kRun;
		}
		if (object->velocity().y > 0.f)
		{
			animation_track_ = (int)PlayerAnimationTrack::kJump;
		}
		if (xmath_util_float3::Length(velocity_xz) >= kPlayerDashSpeed)
		{
			animation_track_ = (int)PlayerAnimationTrack::kDash;
		}

	}
		break;
	case PlayerAnimationTrack::kRun:
		if (IsZero(xmath_util_float3::Length(velocity_xz)))
		{
			animation_track_ = (int)PlayerAnimationTrack::kIdle;
		}		
		if (xmath_util_float3::Length(velocity_xz) >= kPlayerDashSpeed)
		{
			animation_track_ = (int)PlayerAnimationTrack::kDash;
		}
		break;
	case PlayerAnimationTrack::kJump:
		if (object->is_ground())
		{
			animation_track_ = (int)PlayerAnimationTrack::kIdle;
		}
		if (xmath_util_float3::Length(velocity_xz) >= kPlayerDashSpeed)
		{
			animation_track_ = (int)PlayerAnimationTrack::kDash;
		}
		break;
	case PlayerAnimationTrack::kDash:
		if (xmath_util_float3::Length(velocity_xz) < kPlayerDashSpeed)
		{
			animation_track_ = (int)PlayerAnimationTrack::kRun;
		}
		break;
	default:
		break;
	}

	return (int)animation_track_;
}

AnimationState* PlayerAnimationState::GetCopy()
{
    return new PlayerAnimationState(*this);
}
