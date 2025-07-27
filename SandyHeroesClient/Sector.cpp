#include "stdafx.h"
#include "Sector.h"
#include "Object.h"
#include "BoxColliderComponent.h"

Sector::Sector(const std::string& name, const BoundingBox& bounds)
	: name_(name), bounds_(bounds)
{

}

bool Sector::InsertObject(Object* object)
{
	auto in_object = std::find(object_list_.begin(), object_list_.end(), object);
	if (in_object != object_list_.end())
	{
		return true; // 이미 Sector에 있는 오브젝트임 즉, 들어간것과 다를바 없음
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
	if (bounds_.Contains(XMLoadFloat3(&object->position_vector())))
	{
		object_list_.push_back(object);
		return true;
	}
	return false;
}

void Sector::DeleteOutOfBoundsObjects()
{
	object_list_.remove_if([this](Object* object) {
		if(!object->is_movable())
			return false; // 움직이지 않는 오브젝트는 Sector 범위 체크를 하지 않음
		return bounds_.Contains(XMLoadFloat3(&object->position_vector())) == ContainmentType::DISJOINT;
		});

}

void Sector::DeleteObject(Object* object)
{
	object_list_.remove(object);
}

void Sector::DeleteDeadObject()
{
	object_list_.remove_if([](Object* object) {
		return object->is_dead();
		});

}

void Sector::set_bounds(const BoundingBox& bounds)
{
	bounds_ = bounds;
}
