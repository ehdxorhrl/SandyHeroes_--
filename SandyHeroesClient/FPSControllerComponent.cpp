#include "stdafx.h"
#include "FPSControllerComponent.h"
#include "Object.h"
#include "AnimatorComponent.h"
#include "PlayerAnimationState.h"
#include "CameraComponent.h"
#include "MeshComponent.h"
#include "Scene.h"
#include "Mesh.h"
#include "Shader.h"
#include "ModelInfo.h"
#include "Material.h"
#include "AnimationSet.h"
#include "GunComponent.h"
#include "MovementComponent.h"
#include "BaseScene.h"
#include "MeshColliderComponent.h"
#include"GameFramework.h"

FPSControllerComponent::FPSControllerComponent(Object* owner) : InputControllerComponent(owner)
{
	is_key_down_['W'] = false;
	is_key_down_['A'] = false;
	is_key_down_['S'] = false;
	is_key_down_['D'] = false;

	mouse_xy_.x = kDefaultFrameBufferWidth / 2;
	mouse_xy_.y = kDefaultFrameBufferHeight / 2;
}

Component* FPSControllerComponent::GetCopy()
{
    return new FPSControllerComponent(*this);
}

bool FPSControllerComponent::ProcessInput(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time)
{
	switch (message_id)
	{
	case WM_MOUSEMOVE:
	{
		POINT mouse_cursor_pos;
		GetCursorPos(&mouse_cursor_pos);

		RECT client_rect;
		GetClientRect(client_wnd_, &client_rect);

		POINT center;
		center.x = (client_rect.right - client_rect.left) / 2;
		center.y = (client_rect.bottom - client_rect.top) / 2;
		mouse_xy_.x = center.x;
		mouse_xy_.y = center.y;

		ClientToScreen(client_wnd_, &center);

		if (camera_object_)
		{
			camera_object_->Rotate((mouse_cursor_pos.y - center.y) * 0.1, 0, 0);
		}
		SetCursorPos(center.x, center.y);

		GameFramework::Instance()->send_mouse_move_packet(mouse_cursor_pos.x, center.x);
	}
		break;
	case WM_KEYDOWN:
		switch (w_param)
		{
		case VK_SPACE:
		{
			constexpr float kFallCheckVelocity = -0.5;
			if (owner_->is_ground())
			{
				is_jumpkey_pressed_ = true;
			}
		}
			break; 
		case VK_SHIFT:
			if (dash_cool_delta_time_ <= 0)
			{
				is_dash_pressed_ = true;
				dash_cool_delta_time_ = dash_cool_time_;
				dash_velocity_ = { 0,0,0 };
				dash_length_ = 5.0f;
				dash_before_position_ = owner_->position_vector();
				XMFLOAT3 look = owner_->look_vector();
				XMFLOAT3 right = owner_->right_vector();
				look.y = 0.f;
				right.y = 0.f;
				if (is_key_down_['W'])
				{
					dash_velocity_ += look;
				}
				if (is_key_down_['S'])
				{
					dash_velocity_ -= look;
				}
				if (is_key_down_['D'])
				{
					dash_velocity_ += right;
				}
				if (is_key_down_['A'])
				{
					dash_velocity_ -= right;
				}
				if (xmath_util_float3::Length(dash_velocity_) == 0)
				{
					dash_velocity_ += look;
				}
			}
			break;
		default:
			return false;
			break;
		}
		break;
	case WM_KEYUP:
		switch (w_param)
		{
		default:
			return false;
			break;
		}
		break;
	default:
		return false;
		break;
	}
	return true;
}

void FPSControllerComponent::Update(float elapsed_time)
{
	//const auto& movement = Object::GetComponent<MovementComponent>(owner_);
	//
	//XMFLOAT3 velocity{ 0,0,0 };
	//float speed = 5.5f;
	//XMFLOAT3 look = owner_->look_vector();
	//XMFLOAT3 right = owner_->right_vector();
	//look.y = 0.f; // xz 평면을 따라 이동
	//right.y = 0.f; 
	//look = xmath_util_float3::Normalize(look);
	//right = xmath_util_float3::Normalize(right);
	//
	//if (is_key_down_['W']) movement->MoveXZ(look.x, look.z, speed);
	//if (is_key_down_['S']) movement->MoveXZ(-look.x, -look.z, speed);
	//if (is_key_down_['A']) movement->MoveXZ(-right.x, -right.z, speed);
	//if (is_key_down_['D']) movement->MoveXZ(right.x, right.z, speed);
	//
	//if (is_jumpkey_pressed_)
	//{
	//	movement->Jump(jump_speed_);
	//	is_jumpkey_pressed_ = false;
	//}
	//
	//// 대쉬
	//constexpr float kDashSpeed = 35.f;
	//if (is_dash_pressed_)
	//{
	//	is_dash_pressed_ = false;
	//
	//	XMFLOAT3 position = owner_->world_position_vector();
	//
	//	constexpr float kGroundYOffset = 0.75f;
	//	position.y += kGroundYOffset;
	//	XMVECTOR ray_origin = XMLoadFloat3(&position);
	//	position.y -= kGroundYOffset;
	//
	//	XMVECTOR ray_direction = XMLoadFloat3(&dash_velocity_);
	//	ray_direction = XMVectorSetY(ray_direction, 0);
	//	ray_direction = XMVector3Normalize(ray_direction);
	//
	//	if (0 == XMVectorGetX(XMVector3Length(ray_direction)))
	//		return;
	//
	//	float distance{ std::numeric_limits<float>::max() };
	//	BaseScene* base_scene = dynamic_cast<BaseScene*>(scene_);
	//	for (auto& mesh_collider : base_scene->checking_maps_mesh_collider_list(base_scene->stage_clear_num()))
	//	{
	//		float t{};
	//		if (mesh_collider->CollisionCheckByRay(ray_origin, ray_direction, t))
	//		{
	//			if (t < distance)
	//			{
	//				distance = t;
	//			}
	//		}
	//	}
	//
	//	if (distance > 0 && distance < dash_length_)
	//	{
	//		if (distance < 1.5)
	//			dash_length_ = 0.0f;
	//		else
	//			dash_length_ = distance - 1.5f;
	//	}
	//
	//	movement->set_max_speed_xz(kDashSpeed);
	//	movement->MoveXZ(dash_velocity_.x, dash_velocity_.z, kDashSpeed);
	//}
	//
	//
	//if (xmath_util_float3::Length(owner_->position_vector() - dash_before_position_) >= dash_length_)
	//{
	//	movement->set_max_speed_xz(speed);
	//	AnimatorComponent* animator = Object::GetComponent<AnimatorComponent>(owner_);
	//	if (animator)
	//	{
	//		animator->animation_state()->ChangeAnimationTrack((int)PlayerAnimationTrack::kIdle, owner_, animator);
	//	}
	//}
	//dash_cool_delta_time_ -= elapsed_time;

	//if (is_firekey_down_)
	//{
	//	GunComponent* gun = Object::GetComponentInChildren<GunComponent>(owner_);
	//	if (gun && (gun->fire_type() == GunFireType::kAuto))
	//	{
	//		// 1. 총구 위치
	//		XMFLOAT3 gun_shoting_point{ gun->owner()->world_position_vector() };
	//
	//		// 2. 피킹 지점(월드 좌표계)
	//		int sx = mouse_xy_.x;
	//		int sy = mouse_xy_.y;
	//		Object* picked_object = nullptr;
	//
	//		XMVECTOR picking_point_w = XMLoadFloat3(&(camera_object_->world_position_vector() + (camera_object_->world_look_vector() * 100.f)));
	//
	//
	//		// 3. 1번에서 2번을 향하는 총알 발사
	//		XMFLOAT3 bullet_dir{};
	//		XMStoreFloat3(&bullet_dir, XMVector3Normalize(picking_point_w - XMLoadFloat3(&gun_shoting_point)));
	//		auto bullet_mesh = scene_->FindModelInfo("SM_Bullet_01")->GetInstance();
	//		gun->FireBullet(bullet_dir, bullet_mesh, scene_);
	//	}
	//}
}

void FPSControllerComponent::Stop()
{
	is_key_down_['W'] = false;
	is_key_down_['A'] = false;
	is_key_down_['S'] = false;
	is_key_down_['D'] = false;

}	

void FPSControllerComponent::set_camera_object(Object* value)
{
	camera_object_ = value;
}

void FPSControllerComponent::set_scene(Scene* value)
{
	scene_ = value;
}

void FPSControllerComponent::set_particle(ParticleComponent* value)
{
	particle_ = value;
}

bool FPSControllerComponent::is_firekey_down() const
{
	return is_firekey_down_;
}

