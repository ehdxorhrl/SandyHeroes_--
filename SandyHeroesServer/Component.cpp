#include "stdafx.h"
#include "Component.h"
#include "Object.h"

Component::Component(Object* owner) : owner_(owner->shared_from_this())
{
}

Component::Component(const Component& other) : owner_()
{
}

void Component::set_owner(Object* owner)
{
	owner_ = owner->shared_from_this();
}

std::shared_ptr<Object> Component::owner() const
{
	return owner_.lock();
}

std::shared_ptr<Object> Component::hierarchy_root()
{
	if (hierarchy_root_.expired())
	{
		auto locked_owner = owner_.lock();
		if(locked_owner) hierarchy_root_ = locked_owner->GetHierarchyRoot()->shared_from_this();
	}
	return hierarchy_root_.lock();
}
