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

	constexpr float scroll_speed = 1.5f; // 초당 이동 거리

	XMVECTOR direction = XMLoadFloat3(&direction_);
	direction = XMVector3Normalize(direction);

	float delta = scroll_speed * elapsed_time;

	if (moved_distance_ + delta > 1.5f)
	{
		delta = 1.5f - moved_distance_;
		is_active_ = false; // 이동 완료
	}

	XMVECTOR pos = XMLoadFloat3(&owner_->position_vector());
	pos += direction * delta;

	XMFLOAT3 new_pos;
	XMStoreFloat3(&new_pos, pos);
	owner_->set_position_vector(new_pos);

	moved_distance_ += delta;
}
