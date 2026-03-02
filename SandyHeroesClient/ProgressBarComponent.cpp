#include "stdafx.h"
#include "ProgressBarComponent.h"
#include "UiMeshComponent.h"
#include "Object.h"

ProgressBarComponent::ProgressBarComponent(Object* owner)
	: UiComponent(owner)
{
	type_ = UiType::kProgressBarX; // �⺻������ ���� ����� ǥ���ٷ� ����
}

ProgressBarComponent::ProgressBarComponent(const ProgressBarComponent& other)
	: UiComponent(other), max_value_(other.max_value_), current_value_(other.current_value_),
	get_current_value_func_(other.get_current_value_func_), get_max_value_func_(other.get_max_value_func_)
{
}

void ProgressBarComponent::Update(float elapsed_time)
{
	auto locked_owner = owner_.lock();
	if(!locked_owner) return;

	auto ui_mesh = Object::GetComponent<UiMeshComponent>(locked_owner);
	if (!ui_mesh)
	{
		std::string name = locked_owner->name();
		std::wstring wname(name.begin(), name.end());
		wname = L"ProgressBarComponent: " + wname + L"'s UiMeshComponent not found!\n";
		OutputDebugString(wname.c_str());
		return;
	}
	if (!get_current_value_func_)
	{
		std::string name = locked_owner->name();
		std::wstring wname(name.begin(), name.end());
		wname = L"ProgressBarComponent: " + wname + L"'s get_current_value_func_ is not set!\n";
		OutputDebugString(wname.c_str());
		return;
	}
	auto locked_view = view_.lock();
	if (locked_view)
	{
		// view_  ִٸ view_ 簪 
		current_value_ = get_current_value_func_(locked_view);
	}
	else
	{
		// view_    ٸ owner_ 簪 
		current_value_ = get_current_value_func_(locked_owner);
	}
	current_value_ = std::clamp(current_value_, 0.f, max_value_); // 簪 ִ밪 ּҰ ̷ 

	if (!is_correct_max_)
	{
		if (!get_max_value_func_)
		{
			std::string name = locked_owner->name();
			std::wstring wname(name.begin(), name.end());
			wname = L"ProgressBarComponent: " + wname + L"'s get_max_value_func_ is not set!\n";
			OutputDebugString(wname.c_str());
			return;
		}

		if (locked_view)
		{
			max_value_ = get_max_value_func_(locked_view);
		}
		else
		{
			max_value_ = get_max_value_func_(locked_owner);
		}
	}
	float progress = current_value_ / max_value_;

	if (type_ == UiType::kProgressBarY)
	{
		ui_mesh->set_gage_value(XMFLOAT2{ 1.f, progress });
	}
	else
	{
		ui_mesh->set_gage_value(XMFLOAT2{ progress, 1.f });
	}

}
