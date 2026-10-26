#include "stdafx.h"
#include "Sector.h"
#include "Object.h"
#include "BoxColliderComponent.h"

Sector::Sector(const std::string& name, const BoundingBox& bounds)
	: name_(name), bounds_(bounds)
{

}

bool Sector::InsertObject(std::shared_ptr<Object> object)
{
	auto in_object = std::find_if(object_list_.begin(), object_list_.end(), [&object](const std::weak_ptr<Object>& wp) {
		return wp.lock() == object;
	});

	if (in_object != object_list_.end())
	{
		return true;
	}

	auto box_collider = Object::GetComponentInChildren<BoxColliderComponent>(object);
	if (box_collider)
	{
		box_collider->Update(0.f);
		if (bounds_.Contains(box_collider->animated_box()) != ContainmentType::DISJOINT)
		{
			object_list_.push_back(object);
			return true;
		}
	}
	
	XMFLOAT3 pos = object->position_vector();
	if (bounds_.Contains(XMLoadFloat3(&pos)))
	{
		object_list_.push_back(object);
		return true;
	}
	return false;
}

void Sector::DeleteOutOfBoundsObjects()
{
	object_list_.remove_if([this](const std::weak_ptr<Object>& wp) {
		auto object = wp.lock();
		if (!object)
			return true;
		if (!object->is_movable())
			return false;
		
		XMFLOAT3 pos = object->position_vector();
		return bounds_.Contains(XMLoadFloat3(&pos)) == ContainmentType::DISJOINT;
	});
}

void Sector::DeleteObject(Object* object)
{
	object_list_.remove_if([object](const std::weak_ptr<Object>& wp) {
		auto locked = wp.lock();
		return !locked || locked.get() == object;
	});
}

void Sector::set_bounds(const BoundingBox& bounds)
{
	bounds_ = bounds;
}
