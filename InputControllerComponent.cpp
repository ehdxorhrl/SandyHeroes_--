#include "stdafx.h"
#include "InputControllerComponent.h"

InputControllerComponent::InputControllerComponent(Object* owner) : Component(owner)
{

}

void InputControllerComponent::set_client_wnd(HWND value)
{
	client_wnd_ = value;
}
