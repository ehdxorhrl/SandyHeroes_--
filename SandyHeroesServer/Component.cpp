#include "stdafx.h"
#include "Component.h"
#include "Object.h"

Component::Component(Object* owner) : owner_(owner)
{
}

Component::Component(const Component& other) : owner_(nullptr)
{
}

void Component::set_owner(Object* owner)
{
	owner_ = owner;
}

Object* Component::owner() const
{
	return owner_;
}

Object* Component::hierarchy_root()
{
	if (!hierarchy_root_)
		hierarchy_root_ = owner_->GetHierarchyRoot();
	return hierarchy_root_;
}
