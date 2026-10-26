#include "stdafx.h"
#include "ScrollComponent.h"
#include "Object.h"

Component* ScrollComponent::GetCopy()
{
    return new ScrollComponent(*this);
}

void ScrollComponent::Update(float elapsed_time)
{
	if (!is_active_) 
		return;

	constexpr float scroll_speed = 1.5f; // �ʴ� �̵� �Ÿ�

	XMVECTOR direction = XMLoadFloat3(&direction_);
	direction = XMVector3Normalize(direction);

	float delta = scroll_speed * elapsed_time;

	if (moved_distance_ + delta > 1.5f)
	{
		delta = 1.5f - moved_distance_;
		is_active_ = false; //  
	}

	auto locked_owner = owner_.lock();
	if (!locked_owner) return;

	XMVECTOR pos = XMLoadFloat3(&locked_owner->position_vector());
	pos += direction * delta;

	XMFLOAT3 new_pos;
	XMStoreFloat3(&new_pos, pos);
	locked_owner->set_position_vector(new_pos);

	moved_distance_ += delta;
}
