#include "stdafx.h"
#include "PlayerAnimationState.h"
#include "Object.h"
#include "MovementComponent.h"
#include "AnimatorComponent.h"

void PlayerAnimationState::Enter(int animation_track, Object* object, AnimatorComponent* animator)
{
	switch ((PlayerAnimationTrack)animation_track)
	{
	case PlayerAnimationTrack::kJump:
		//점프시 플레이어의 root node가 컨트롤러에 의해 이동하므로 애니메이션에 의해 메쉬의 위치가 변하면 안됨
		animator->set_is_ignore_root_bone_traslation(true); 
		break;
	case PlayerAnimationTrack::kDash:
		animator->set_speed_scale(2.f);
		break;
	}
}

int PlayerAnimationState::Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)
{
	animator->set_speed_scale(1.f);
	constexpr float kPlayerDashSpeed = 70.f;
	auto movement = Object::GetComponentInChildren<MovementComponent>(object);
	auto velocity_xz = movement->velocity();
	velocity_xz.y = 0.f;

	switch ((PlayerAnimationTrack)animation_track())
	{
	case PlayerAnimationTrack::kIdle:
	{
		if (xmath_util_float3::Length(velocity_xz) > 0)
		{
			ChangeAnimationTrack((int)PlayerAnimationTrack::kRun, object, animator);
		}
		if (movement->velocity().y > 0.f)
		{
			ChangeAnimationTrack((int)PlayerAnimationTrack::kJump, object, animator);
		}
		if (xmath_util_float3::Length(velocity_xz) >= kPlayerDashSpeed)
		{
			ChangeAnimationTrack((int)PlayerAnimationTrack::kDash, object, animator);
		}

	}
		break;
	case PlayerAnimationTrack::kRun:
		if (IsZero(xmath_util_float3::Length(velocity_xz)))
		{
			ChangeAnimationTrack((int)PlayerAnimationTrack::kIdle, object, animator);
		}		
		if (xmath_util_float3::Length(velocity_xz) >= kPlayerDashSpeed)
		{
			ChangeAnimationTrack((int)PlayerAnimationTrack::kDash, object, animator);
		}
		break;
	case PlayerAnimationTrack::kJump:
		if (object->is_ground())
		{
			ChangeAnimationTrack((int)PlayerAnimationTrack::kIdle, object, animator);
		}
		if (xmath_util_float3::Length(velocity_xz) >= kPlayerDashSpeed)
		{
			ChangeAnimationTrack((int)PlayerAnimationTrack::kDash, object, animator);

		}
		break;
	case PlayerAnimationTrack::kDash:
		if (xmath_util_float3::Length(velocity_xz) < kPlayerDashSpeed)
		{
			ChangeAnimationTrack((int)PlayerAnimationTrack::kRun, object, animator);
		}
		break;
	default:
		break;
	}

	return animation_track();
}

void PlayerAnimationState::Exit(int animation_track, Object* object, AnimatorComponent* animator)
{
	switch ((PlayerAnimationTrack)animation_track)
	{
	case PlayerAnimationTrack::kJump:
		animator->set_is_ignore_root_bone_traslation(false);
		break;
	case PlayerAnimationTrack::kDash:
		animator->set_speed_scale(1.f);
		break;
	}
}

AnimationState* PlayerAnimationState::GetCopy()
{
    return new PlayerAnimationState(*this);
}
