#include "stdafx.h"
#include "Sector.h"
#include "Object.h"
#include "BoxColliderComponent.h"
#include "MeshComponent.h"

Sector::Sector(const std::string& name, const BoundingBox& bounds)
	: name_(name), bounds_(bounds)
{

}

bool Sector::InsertMeshComponent(const std::shared_ptr<MeshComponent>& mesh_component)
{
	if (CheckObjectInSectorMeshComponentList(mesh_component))
	{
		return true;
	}

	BoundingOrientedBox obb;
	auto aabb = mesh_component->GetMesh()->bounds();
	BoundingOrientedBox::CreateFromBoundingBox(obb, aabb);
	obb.Transform(obb, XMLoadFloat4x4(&mesh_component->owner()->world_matrix()));

	if (bounds_.Contains(obb) != ContainmentType::DISJOINT)
	{
		mesh_component_list_.push_back(mesh_component);
		return true;
	}

	return false;
}

void Sector::DeleteOutOfBounds()
{
	mesh_component_list_.remove_if([this](const std::weak_ptr<MeshComponent>& wp) {
		auto mesh_component = wp.lock();
		if (!mesh_component)
			return true;

		BoundingOrientedBox obb;
		auto aabb = mesh_component->GetMesh()->bounds();
		BoundingOrientedBox::CreateFromBoundingBox(obb, aabb);
		obb.Transform(obb, XMLoadFloat4x4(&mesh_component->owner()->world_matrix()));

		return bounds_.Contains(obb) == ContainmentType::DISJOINT;
	});
}

void Sector::DeleteMeshComponent(const std::shared_ptr<MeshComponent>& mesh_component)
{
	mesh_component_list_.remove_if([mesh_component](const std::weak_ptr<MeshComponent>& wp) {
		auto locked = wp.lock();
		return !locked || locked == mesh_component;
	});
}

void Sector::set_bounds(const BoundingBox& bounds)
{
	bounds_ = bounds;
}
