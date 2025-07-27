#include "stdafx.h"
#include "BoxColliderComponent.h"
#include "Object.h"

BoxColliderComponent::BoxColliderComponent(Object* owner, const BoundingBox& box)
	: Component(owner)
{
	BoundingOrientedBox::CreateFromBoundingBox(box_, box);
}
BoxColliderComponent::BoxColliderComponent(Object* owner, const BoundingOrientedBox& box)
	: Component(owner)
{
	box_ = box;
	animated_box_ = box;
}

BoxColliderComponent::BoxColliderComponent(const BoxColliderComponent& other) : Component(other)
{
	box_ = other.box_;
	animated_box_ = other.animated_box_;
}

Component* BoxColliderComponent::GetCopy()
{
    return new BoxColliderComponent(*this);
}

void BoxColliderComponent::Update(float elapsed_time)
{
	// Update the animated box based on the current world matrix of the owner object
	box_.Transform(animated_box_, XMLoadFloat4x4(&owner_->world_matrix()));
}

void BoxColliderComponent::set_box(const BoundingOrientedBox& box)
{
	box_ = box;
}

const BoundingOrientedBox& BoxColliderComponent::box() const
{
	return box_;
}

BoundingOrientedBox BoxColliderComponent::animated_box() const
{
	return animated_box_;
}
