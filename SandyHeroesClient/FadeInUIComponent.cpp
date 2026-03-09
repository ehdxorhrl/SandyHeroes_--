#include "stdafx.h"
#include "FadeInUIComponent.h"
#include "UiMeshComponent.h"
#include "Object.h"

FadeInUIComponent::FadeInUIComponent(Object* owner, float duration)
	: Component(owner), duration_(duration)
{
	ui_mesh_ = Object::GetComponent<UiMeshComponent>(owner_.lock());
	auto locked_ui_mesh = ui_mesh_.lock();
	if (locked_ui_mesh)
	{
		locked_ui_mesh->set_alpha(0.0f); // 처음은 투명
		locked_ui_mesh->set_is_visible(true); // 혹시라도 비활성화돼 있으면 켜기
	}
}
FadeInUIComponent::FadeInUIComponent(const std::shared_ptr<Object>& owner, float duration)
	: Component(owner), duration_(duration)
{
	ui_mesh_ = Object::GetComponent<UiMeshComponent>(owner_.lock());
	auto locked_ui_mesh = ui_mesh_.lock();
	if (locked_ui_mesh)
	{
		locked_ui_mesh->set_alpha(0.0f); // 처음은 투명
		locked_ui_mesh->set_is_visible(true); // 혹시라도 비활성화돼 있으면 켜기
	}
}

FadeInUIComponent::FadeInUIComponent(const FadeInUIComponent& other)
	: Component(other), duration_(other.duration_), elapsed_(other.elapsed_)
{
	// 복사 생성자에서는 owner_가 아직 없기 때문에 Init은 따로 해줘야 함
}

Component* FadeInUIComponent::GetCopy()
{
	return new FadeInUIComponent(*this);
}

void FadeInUIComponent::Update(float elapsed_time)
{
	auto locked_ui_mesh = ui_mesh_.lock();
	if (!locked_ui_mesh)
	{
		auto locked_owner = owner_.lock();
		if (!locked_owner) return;
		ui_mesh_ = Object::GetComponent<UiMeshComponent>(locked_owner);
	}

	elapsed_ += elapsed_time;
	float alpha = std::min(1.0f, elapsed_ / duration_);
	locked_ui_mesh->set_alpha(alpha);
}