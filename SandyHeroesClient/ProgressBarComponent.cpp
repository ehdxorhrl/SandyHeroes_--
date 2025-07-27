#include "stdafx.h"
#include "ProgressBarComponent.h"
#include "UiMeshComponent.h"
#include "Object.h"

ProgressBarComponent::ProgressBarComponent(Object* owner)
	: UiComponent(owner)
{
	type_ = UiType::kProgressBarX; // 기본적으로 가로 진행률 표시줄로 설정
}

ProgressBarComponent::ProgressBarComponent(const ProgressBarComponent& other)
	: UiComponent(other), max_value_(other.max_value_), current_value_(other.current_value_),
	get_current_value_func_(other.get_current_value_func_), get_max_value_func_(other.get_max_value_func_)
{
}

void ProgressBarComponent::Update(float elapsed_time)
{
	UiMeshComponent* ui_mesh = Object::GetComponent<UiMeshComponent>(owner_);
	if (!ui_mesh)
	{
		std::string name = owner_->name();
		std::wstring wname(name.begin(), name.end());
		wname = L"ProgressBarComponent: " + wname + L"'s UiMeshComponent not found!\n";
		OutputDebugString(wname.c_str());
		return;
	}
	if (!get_current_value_func_)
	{
		std::string name = owner_->name();
		std::wstring wname(name.begin(), name.end());
		wname = L"ProgressBarComponent: " + wname + L"'s get_current_value_func_ is not set!\n";
		OutputDebugString(wname.c_str());
		return;
	}
	if (view_)
	{
		// view_가 설정되어 있다면 view_의 현재값을 가져옴
		current_value_ = get_current_value_func_(view_);
	}
	else
	{
		// view_가 설정되어 있지 않다면 owner_의 현재값을 가져옴
		current_value_ = get_current_value_func_(owner_);
	}
	current_value_ = std::clamp(current_value_, 0.f, max_value_); // 현재값을 최대값과 최소값 사이로 제한

	if (!is_correct_max_)
	{
		if (!get_max_value_func_)
		{
			std::string name = owner_->name();
			std::wstring wname(name.begin(), name.end());
			wname = L"ProgressBarComponent: " + wname + L"'s get_max_value_func_ is not set!\n";
			OutputDebugString(wname.c_str());
			return;
		}

		if (view_)
		{
			max_value_ = get_max_value_func_(view_);
		}
		else
		{
			max_value_ = get_max_value_func_(owner_);
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
