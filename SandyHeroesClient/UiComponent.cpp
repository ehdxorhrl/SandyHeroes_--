#include "stdafx.h"
#include "UiComponent.h"
#include "UiMeshComponent.h"

UiComponent::UiComponent(Object* owner)
	: Component(owner)
{
}

UiComponent::UiComponent(Object* owner, UiType type)
	: Component(owner), type_(type)
{

}

UiComponent::UiComponent(const UiComponent& other)
	: Component(other), type_(other.type_)
{
}

Component* UiComponent::GetCopy()
{
	return new UiComponent(*this);
}
