#include "stdafx.h"
#include "PlayerAnimationState.h"
#include "Object.h"
#include "MovementComponent.h"
#include "AnimatorComponent.h"
#include "SessionManager.h"

void send_player_animation(uint8_t animation_track, Object* object) {

	auto movement = Object::GetComponentInChildren<MovementComponent>(object);
	auto velocity = movement->velocity();

	sc_packet_player_change_animation pca;
	pca.size = sizeof(sc_packet_player_change_animation);
	pca.type = S2C_P_PLAYER_CHANGE_ANIMATION;
	pca.id = object->id();
	pca.animation_track = animation_track;
	pca.loop_type = 1;
	pca.vx = velocity.x;
	pca.vy = velocity.y;
	pca.vz = velocity.z;

	std::cout << animation_track << std::endl;

	const auto& users = SessionManager::getInstance().getAllSessions();
	for (auto& u : users) {
		u.second->do_send(&pca);
	}
}

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

int PlayerAnimationState::Run(Object* object, bool is_end, AnimatorComponent* animator)
{
	animator->set_speed_scale(1.f);
	constexpr float kPlayerDashSpeed = 70.f;
	auto movement = Object::GetComponentInChildren<MovementComponent>(object);
	auto velocity_xz = movement->velocity();
	velocity_xz.y = 0.f;

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
