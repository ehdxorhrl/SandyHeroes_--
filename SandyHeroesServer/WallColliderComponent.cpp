#include "stdafx.h"
#include "WallColliderComponent.h"
#include "BoxColliderComponent.h"

bool WallColliderComponent::CollisionCheckByRay(FXMVECTOR ray_origin, FXMVECTOR ray_direction, float& out_distance)
{
	if (!box_collider_)
	{
		return MeshColliderComponent::CollisionCheckByRay(ray_origin, ray_direction, out_distance);
	}
	else
	{
		auto box = box_collider_->animated_box();
		return box.Intersects(ray_origin, ray_direction, out_distance);
	}
}
