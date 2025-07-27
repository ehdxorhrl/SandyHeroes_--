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
