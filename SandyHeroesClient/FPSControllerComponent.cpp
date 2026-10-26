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
#include "GameFramework.h"
#include "PlayerComponent.h"
#include "WallColliderComponent.h"

FPSControllerComponent::FPSControllerComponent(Object* owner) : InputControllerComponent(owner)
{
	is_key_down_['W'] = false;
	is_key_down_['A'] = false;
	is_key_down_['S'] = false;
	is_key_down_['D'] = false;

	mouse_xy_.x = kDefaultFrameBufferWidth / 2;
	mouse_xy_.y = kDefaultFrameBufferHeight / 2;
}
FPSControllerComponent::FPSControllerComponent(const std::shared_ptr<Object>& owner) : InputControllerComponent(owner)
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
	auto owner = owner_.lock();
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

		if (auto camera_object = camera_object_.lock())
		{
			camera_object->Rotate((mouse_cursor_pos.y - center.y) * 0.1, 0, 0);
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
			if (!owner) return false;
			if (owner->is_ground())
			{
				is_jumpkey_pressed_ = true;
			}
		}
			break; 
		case VK_SHIFT:
			if (dash_cool_delta_time_ <= 0)
			{
				if (!owner) return false;
				is_dash_pressed_ = true;
				dash_cool_delta_time_ = dash_cool_time_;
				dash_velocity_ = { 0,0,0 };
				dash_length_ = 5.0f;
				dash_before_position_ = owner->position_vector();
				XMFLOAT3 look = owner->look_vector();
				XMFLOAT3 right = owner->right_vector();
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
}

void FPSControllerComponent::Stop()
{
	is_key_down_['W'] = false;
	is_key_down_['A'] = false;
	is_key_down_['S'] = false;
	is_key_down_['D'] = false;

}	

void FPSControllerComponent::set_camera_object(std::shared_ptr<Object> value)
{
	camera_object_ = value;
}

void FPSControllerComponent::set_scene(Scene* value)
{
	scene_ = value;
}

void FPSControllerComponent::set_particle(std::shared_ptr<ParticleComponent> value)
{
	particle_ = value;
}

bool FPSControllerComponent::is_firekey_down() const
{
	return is_firekey_down_;
}

