#include "stdafx.h"
#include "Object.h"
#include "TestControllerComponent.h"
#include "MovementComponent.h"

TestControllerComponent::TestControllerComponent(Object* owner)
	: InputControllerComponent(owner)
{
	is_key_down_['W'] = false;
	is_key_down_['A'] = false;
	is_key_down_['S'] = false;
	is_key_down_['D'] = false;
	is_key_down_['Q'] = false;
	is_key_down_['E'] = false;
	is_key_down_[VK_LBUTTON] = false;
	is_key_down_[VK_SHIFT] = false;
}

Component* TestControllerComponent::GetCopy()
{
	return new TestControllerComponent(*this);
}

bool TestControllerComponent::ProcessInput(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time)
{
	switch (message_id)
	{
	case WM_MOUSEWHEEL:
	{
		int delta = GET_WHEEL_DELTA_WPARAM(w_param);
		if (delta > 0)
		{
			move_speed_ += 5.f;
		}
		else
		{
			move_speed_ -= 5.f;
		}
		if (move_speed_ < move_min_speed_)
		{
			move_speed_ = move_min_speed_;
		}
		if (move_speed_ > move_max_speed_)
		{
			move_speed_ = move_max_speed_;
		}
	}
		break;
	case WM_MOUSEMOVE:
		if (is_key_down_[VK_LBUTTON])
		{
			int x = LOWORD(l_param), y = HIWORD(l_param);
			owner_->Rotate((y - mouse_xy_.y) * rotate_speed_, (x - mouse_xy_.x) * rotate_speed_, 0.f);

			RECT client_rect;
			GetClientRect(client_wnd_, &client_rect);

			POINT center;
			center.x = (client_rect.right - client_rect.left) / 2;
			center.y = (client_rect.bottom - client_rect.top) / 2;
			mouse_xy_.x = center.x;
			mouse_xy_.y = center.y;

			ClientToScreen(client_wnd_, &center);

			SetCursorPos(center.x, center.y);

		}
		break;
	case WM_LBUTTONDOWN:
		if (!is_key_down_[VK_LBUTTON])
		{
			ShowCursor(false);
			SetCapture(client_wnd_);
			is_key_down_[VK_LBUTTON] = true;
			mouse_xy_.x = LOWORD(l_param);
			mouse_xy_.y = HIWORD(l_param);
		}
		break;
	case WM_LBUTTONUP:
		if (is_key_down_[VK_LBUTTON])
		{
			ShowCursor(true);
			ReleaseCapture();
			is_key_down_[VK_LBUTTON] = false;
		}
		break;
	case WM_KEYDOWN:
		switch (w_param)
		{
		case 'W':
			if (!is_key_down_['W'])
			{
				is_key_down_['W'] = true;
					
			}
			break;
		case 'A':
			if (!is_key_down_['A'])
			{
				is_key_down_['A'] = true;
			}
			break;
		case 'S':
			if (!is_key_down_['S'])
			{
				is_key_down_['S'] = true;
			}
			break;
		case 'D':
			if (!is_key_down_['D'])
			{
				is_key_down_['D'] = true;
			}
			break;
		case 'Q':
			if (!is_key_down_['Q'])
			{
				is_key_down_['Q'] = true;
			}
			break;
		case 'E':
			if (!is_key_down_['E'])
			{
				is_key_down_['E'] = true;
			}
			break;
		case VK_SHIFT:
			is_key_down_[VK_SHIFT] = true;
			break;
		default:
			return false;
			break;
		}
		break;
	case WM_KEYUP:
		switch (w_param)
		{
		case 'W':
			if (is_key_down_['W'])
			{
				is_key_down_['W'] = false;
			}
			break;
		case 'A':
			if (is_key_down_['A'])
			{
				is_key_down_['A'] = false;
			}
			break;
		case 'S':
			if (is_key_down_['S'])
			{
				is_key_down_['S'] = false;
			}
			break;
		case 'D':
			if (is_key_down_['D'])
			{
				is_key_down_['D'] = false;
			}
			break;
		case 'Q':
			if (is_key_down_['Q'])
			{
				is_key_down_['Q'] = false;
			}
			break;
		case 'E':
			is_key_down_['E'] = false;
			break;
		case VK_SHIFT:
			is_key_down_[VK_SHIFT] = false;
			break;
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

void TestControllerComponent::Update(float elapsed_time)
{
	auto movement = Object::GetComponentInChildren<MovementComponent>(owner_);
	if (!movement)
	{
		return;
	}
	
	movement->Stop();

	XMFLOAT3 velocity{ 0,0,0 };
	XMFLOAT3 look = owner_->look_vector();
	XMFLOAT3 right = owner_->right_vector();
	XMFLOAT3 up{ 0,1,0 };
	float speed_rate = 1.f;
	if (is_key_down_['W']) velocity += look;
	if (is_key_down_['S']) velocity -= look;
	if (is_key_down_['A']) velocity -= right;
	if (is_key_down_['D']) velocity += right;
	if (is_key_down_['E']) velocity += up;
	if (is_key_down_['Q']) velocity -= up;

	if (is_key_down_[VK_SHIFT])
	{
		speed_rate = 2.f;
	}
	movement->set_max_speed_xz(move_speed_ * speed_rate);
	movement->set_max_speed(move_speed_ * speed_rate);
	movement->Move(velocity, move_speed_ * speed_rate);
}
